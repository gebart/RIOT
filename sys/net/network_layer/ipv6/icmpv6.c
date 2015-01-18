





void icmpv6_process(pkt_t *pkt)
{
    icmpv6_hdr_t *hdr;
    netmod_t target;

    /* mark the icmp header */
    pkt->next->type = NETMOD_ICMPV6;
    hdr = (icmpv6_hdr_t *)pkt->next->payload;

    /* multiplex ICMP packet */
    switch (hdr->type) {
        case ICMPV6_TYPE_RPL_DIO:
            target = NETMOD_RPL;
            break;
    }

    /* find out who is interested in the packet and send them a pointer */
    mst_t msg;
    msg.type = NETAPI_CMD_RCV;
    mst.content.ptr = (char *)pkt;

    receiver = netreg_lookup(netreg, tartget);
    if (receiver != KERNEL_PID_UNKNWON) {
        msg_send(&msg, receiver);
        while (receiver = netreg_getnext(netreg)) {
            pktbuf_hold(&hdr_pkt);
            msg_send(&msg, receiver);
        }
    }

    netapi_send(NETAPI_CMD_RCV);
}
