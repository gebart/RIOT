


static const uint8_t stepper_lp_seq[] = {0x01, 0x04, 0x02, 0x08};
static const uint8_t stepper_hp_seq[] = {0x09, 0x05, 0x06, 0x0a};


int l298n_init(l298n_t *dev, gpio_t in1, gpio_t in2, gpio_t in3, gpio_t in4)
{
    /* save pins */
    dev->pins[0] = in1;
    dev->pins[1] = in2;
    dev->pins[2] = in3;
    dev->pins[3] = in4;
}

int stepper_set_pos(stepper_t *dev, int angle)
{
    msg_t msg;
    msg.type = STEPPER_MSG_POS;
    msg.content.ptr = &angle;
}


int l298_step(l298n_t *dev, int dir)
{
    if (dir) {      /* go forwards */
        dev->seq = (++dev->seq) & 0x03;
        dev->pos++;
    }
    else {          /* move backwards */
        dev->seq = (--dev->seq) & 0x03;
        dev->pos--;
    }
    _step(dev);
}


void _step(stepper_t *dev, int dir)
{
    dev->seq += dir;
    for (int i = 0; i < 4; i++) {
        gpio_write(dev->pins[i], stepper_seq[dev->seq] & (1 << i));
    }
}

void _stepper_thread(void *arg)
{
    stepper_t *dev = (stepper_t *)arg;
    mst_t msg;
    int set_pos;
    int pos;
    int dir;

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case STEPPER_MSG_POS_FW:
                set_pos = *(msg.content.ptr);
                dir = 1;
                break;
            case STEPPER_MSG_POS_BW:
                set_pos = *(msg.content.ptr);
                dir = -1;
                break;

            case STEPPER_MSG_MOVE_FW:
                set_pos += *(msg.content.ptr);
                dir = 1;
                _move();
                break;
            case STEPPER_MSG_MOVE_BW:
                set_pos += *(msg.content.ptr);
                dir = -1;
                _move();
                break;

            case TIMER:
                /* do nothing */
                if ()
                stepper_step(dev, dir);
                pos += dir;
                if (pos != set_pos) {
                    vtimer_set_msg();
                }

        }

        if (dir) {

        }
    }
}
