


typedef struct {
    gpio_t pins[4];
    uint8_t seq;
    int speed;
    kernel_pid_t pid;
} l298n_t;

typedef enum {
    STEPPER_FW = 1,
    STEPPER_BW = -1
} stepper_dir_t;



int l298n_init(l298n_t *dev, gpio_t in1, gpio_t in2, gpio_t in3, gpio_t in4
               int steps);


int l298_step(l298n_t *dev, int dir);

/**
 * @brief Set the movement speed in degrees per second.
 *
 * @param dev [description]
 * @param speed [description]
 *
 * @return [description]
 */
int stepper_set_speed(stepper_t *dev, int speed);

int stepper_move(stepper_t *dev, int angle);
int stepper_set_pos(stepper_t *dev, int angle);
