





/**
 * @brief Generic link layer frame format
 */
typedef struct {
    uint8_t addr_len;
    uint8_t rssi;
    uint8_t lqi;
    uint8_t reserved;
    uint8_t* addresses;
} ll_gen_frame_t;
