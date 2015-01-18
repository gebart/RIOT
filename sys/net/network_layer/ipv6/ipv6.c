


static int running = 1;
static char stack[KERNEL_CONF_STASKSIZE_DEFAULT];



void _send_packet(pkt_t *pkt)
{
    ipv6_hdr_t *hdr;
    pkt_t *ll_header;
    nc_entry_t *interface;
    msg_t msg;
    ipv6_addr_t addr;

    /* get the IP header, if the given packet is not IPv6, throw it away */
    if (pkt.type != NETMOD_IPV6) {
        pktbuf_release(pkt);
        DEBUG("IPv6: send_packet: got packet without correct IPv6 header, dropped it");
        return;
    }
    else {
        hdr = (ipv6_pkt_t *)pkt->payload;
    }

    /* check if next header is set */
    if (hdr->nextheader == 0) {
        pktbuf_release(pkt);
        DEBUG("IPv6: send_packet: upper layer missed to fill out nextheader field, dropping packet");
    }

    /* the interesting part: find out on which interface we need to send */
    /* 1. what is the next hop -> ask forwarding table */
    /* 2. ask neighbor table about link layer information for the next hop */
    fwtable_next_hop(&hdr->dst_addr, &addr);
    if (addr == NULL) {
        /* no next hop could be found -> what do we do now? Just drop the packet? */
        /* Or do we initialize some kind of routing actions? */
        DEBUG("IPv6: send_packet: unable to locate next hop, dropping packet for now");
        pktbuf_release(pkt);
        return;
    }
    /* find out on what interface the packet is to be send */
    interface = nc_lookup(&addr);
    if (interface == NULL) {
        pktbuf_release(pkt);
        DEBUG("IPv6: send_packet: error during interface lookup, dropping packet");
        return;
    }

    /* fill fields of the IP header */
    ipv6_hdr_set_src_addr(hdr, interface->src_addr);
    ipv6_hdr_set_length(hdr, pktbuf_sizeof(pkt));

    /* now the interesting part: prepend the correct link-layer header */
    /* for this we ask the registry to create the header for us! */
    ll_header = netreg_get_ll_header(interface->id, &interface->dst_addr, interface->ll_addr_len);
    if (ll_header == NULL) {
        pktbuf_release(pkt);
        DEBUG("IPv6: send_packet: unable to create link layer header, dropping packet");
        return;
    }

    /* put link layer header in place */
    ll_header->next = pkt;

    /* and finally pass everything to the link layer */
    msg.type = NETAPI_CMD_SND;
    mst.content.ptr = (char *)ll_header;
}

void _process(pkt_t *pkt)
{

    /* mark the packets payload */
    pkt = pktbuf_allocate(0, NETMOD_UNKNOWN);
    pkt->payload = hdr_pkt->payload + IPV6_HDR_LENGTH;
    pkt->size = hdr_pkt->size - IPV6_HDR_LENGTH;
    hdr_pkt->next = pkt;

    /* lets find out what this packet contains */
    switch (hdr->nextheader) {
        case IPV6_PROTO_NUM_ICMPV6:
            icmpv6_process(hdr_pkt);
            return;
        case IPV6_PROTO_NUM_UDP:
            netmod = NETMOD_UDP;
            break;
        case IPV6_PROTO_NUM_TCP:
            netmod = NETMOD_TCP;
            break;
        default:
            /* no idea what the payload means: drop it! */
            pktbuf_release(hdr_pkt);
            DEBUG("IPv6: receive_packet: got packet with unknown next header, dropped it");
            return;
    }

    /* find out who is interested in the packet and send them a pointer */
    mst_t msg;
    msg.type = NETAPI_CMD_RCV;
    mst.content.ptr = (char *)hdr_pkt;

    receiver = netreg_lookup(netreg, netmod);
    if (receiver != KERNEL_PID_UNKNWON) {
        msg_send(&msg, receiver);
        while (receiver = netreg_getnext(netreg)) {
            pktbuf_hold(&hdr_pkt);
            msg_send(&msg, receiver);
        }
    }
}

void _receive(pkt_t *pkt)
{
    msg_t;
    pkt_t *hdr_pkt;
    ipv6_hdr_t *hdr;
    int netmod;
    kernel_pid_t receiver;
    netreg_entry_t *netreg;

    /* we only care about the IP header, so drop the previous one */
    hdr_pkt = pkt->next;
    pkt->next = NULL;
    pktbuf_release(pkt);

    /* check if we really have in IP header */
    if (hdr_pkt == NULL) {
        DEBUG("IPv6: receive_packet: got empty packet");
        return;
    }
    hdr = (ipv6_hdr_t *)hdr_pkt->payload;
    if (hdr_pkt->size < IPV6_HDR_LENGTH || hdr->version_trafficclass != IPV6_VER) {
        pktbuf_release(hdr_pkt);
        DEBUG("IPv6: receive_packet: received packet is not an IPv6 packet, dropping it");
        return;
    }
    hdr_pkt->type = NETMOD_IPV6;

    /* find out if the packet is send to us or if we need to forward it */
    if (!netreg_is_that_me(&pkt->dest_addr)) {
        _send_packet(hdr_pkt);
        return;
    }
    else {
        _process_packet(hdr_pkt);
    }
}


void *_event_loop(void *arg)
{
    msg_t input;

    while (running) {
        /* listen for incoming NETAPI messages */
        msg_receive(&msg);

        switch (msg.type) {
            case NETAPI_CMD_SND:
                _send_packet((pkt_t *)msg.content.ptr);
                break;
            case NETAPI_CMD_RCV:
                _receive((pkt_t *)msg.content.ptr);
                break;
            case NETAPI_CMD_GET:
                _get_option(&msg);
            default:
                /* no idea what to do, so do nothing */
                break;
        }
    }
}

/* should we pass stack and stuff here? I think its not needed as we run only one instance of IPv6 */
int ipv6_init(char thread_priority)
{
    kernel_pid_t pid;

    /* start IP thread */
    pid = thread_create(stack, KERNEL_CONF_STASKSIZE_DEFAULT, thread_priority, NULL,
                        _event_loop, NULL, "IPv6");

    /* register IP with netreg */
    netreg_register(NETMOD_IPV6, ipv6_create_header);
}
