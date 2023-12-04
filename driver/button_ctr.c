
/*
** @This file based on button.c of Derek Molloy and has modified
** @to control 4 buttons
** @Link: https://github.com/sdwuyawen/BeagleBoneBlack/blob/master/button/button.c
** @Author: vuhailongkl97@gmail.com
** 	    Tungnt58 (Add send signal to userspace functionality)
*/
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <asm/siginfo.h>

#define ROW_1_Pin 67
#define ROW_2_Pin 68
#define ROW_3_Pin 44
#define ROW_4_Pin 61
#define COL_1_Pin 46
#define COL_2_Pin 65
#define COL_3_Pin 27
#define COL_4_Pin 26
#define MY_SIGNAL		40


static unsigned int gpioButton_COL[4] = {46, 65, 27, 26};
static unsigned int gpioButton_ROW[4] = {67, 68, 44, 61};
/* up down left right */
static unsigned int irqNumber[4];

static int button_status = 0;
static struct task_struct *t;
char key_pressed = 0;
static irq_handler_t  button_irq_handler(unsigned int irq, void *dev_id,
						 struct pt_regs *regs);

static int dev_open(struct inode *, struct file *);
static int dev_close(struct inode *, struct file *);

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_close,
};

static struct miscdevice btn_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "buttons",
	.fops = &fops,
};

/* This function to get user process id */
static int dev_open(struct inode *inodep, struct file *filep)
{
	struct pid *current_pid;
	pid_t user_pid;

	current_pid = get_task_pid(current, PIDTYPE_PID);
	t = pid_task(current_pid, PIDTYPE_PID);
	user_pid = pid_nr(current_pid);
	pr_info("user process id is %d\n", user_pid);

	return 0;
}

static int dev_close(struct inode *inodep, struct file *filep)
{
	/* Do Nothing */
	return 0;
}


/* Old way for polling on userspace*/
/*static ssize_t dev_read(struct file *filep, char __user *buf, size_t len,
			loff_t *offset)
{
	char temp_str[5];
	if (count)
		return 0;
	count++;
	sprintf(temp_str, "%d", button_status);
	copy_to_user(buf, temp_str, strlen(temp_str));
	return strlen(temp_str);
}*/

static int __init button_init(void)
{
	int result = 0, i = 0;
	pr_info("GPIO_TEST: Initializing the GPIO_TEST LKM\n");

	for (i = 0; i< 4; i++) {
		gpio_request(gpioButton_COL[i], "sysfs");
        gpio_request(gpioButton_ROW[i], "sysfs");

		gpio_direction_input(gpioButton_COL[i]);
        gpio_direction_output(gpioButton_ROW[i],1);

//		gpio_set_debounce(gpioButton_COL[i], 50);

		gpio_export(gpioButton_COL[i], false);
        gpio_export(gpioButton_ROW[i], false);

		pr_info("GPIO_TEST: The button state is currently: %d\n",
				gpio_get_value(gpioButton_COL[i]));
        pr_info("GPIO_TEST_ROW: The button state is currently: %d\n",
				gpio_get_value(gpioButton_ROW[i]));

		irqNumber[i] = gpio_to_irq(gpioButton_COL[i]);
		pr_info("GPIO_TEST: The button is mapped to IRQ: %d\n",
							irqNumber[i]); 

		result = request_irq(irqNumber[i],
				(irq_handler_t) button_irq_handler,
				IRQF_TRIGGER_RISING,
				"ebb_gpio_handler",
				NULL);
 
		pr_info("GPIO_TEST: The interrupt request result is: %d\n",
							result);
		if (result != 0) {
			pr_info("error request irq\n");
			return -1;
		}
	}
	result = misc_register(&btn_dev);
	if (result) {
		pr_info("can't not register device\n");
		return result;
	}
	pr_info("register successfully\n");

	return 0;
}
 

static void __exit button_exit(void)
{
	int i = 0;

	pr_info("GPIO_TEST: The button state is currently: %d\n",
		gpio_get_value(gpioButton_COL[i]));
	for (i = 0; i< 4; i++) {
		free_irq(irqNumber[i], NULL);             
		gpio_unexport(gpioButton_COL[i]);  
        gpio_unexport(gpioButton_ROW[i]);             
		gpio_free(gpioButton_COL[i]); 
        gpio_free(gpioButton_ROW[i]);                    
	}

	misc_deregister(&btn_dev);
	pr_info("GPIO_TEST: Goodbye from the LKM!\n");
}

/* Send signale to userspace via user process id */
static void send_sig_to_user(int sig_val)
{
	struct kernel_siginfo info;
	info.si_signo = MY_SIGNAL;
	info.si_code = SI_QUEUE;
	info.si_int = sig_val;

	if (t != NULL) {
		if (send_sig_info(MY_SIGNAL, &info, t) < 0)
			pr_err("send signal failed\n");
	} else {
		pr_err("pid_task error\n");
	}
}
char keypad_scan(int gpio_pin)
{   
    int i = 0;
    char keys[4][4] = {{'1', '2', '3', 'A'},
                     {'4', '5', '6', 'B'},
                     {'7', '8', '9', 'C'},
                     {'*', '0', '#', 'D'}};
                
    for (i = 0; i< 4; i++) {
    // Set current column as output and low
        switch(i)
        {
            case 0:
                gpio_set_value(ROW_1_Pin, 0);
                gpio_set_value(ROW_2_Pin, 1);
                gpio_set_value(ROW_3_Pin, 1);
                gpio_set_value(ROW_4_Pin, 1);
            break;
            case 1:
                gpio_set_value(ROW_1_Pin, 1);
                gpio_set_value(ROW_2_Pin, 0);
                gpio_set_value(ROW_3_Pin, 1);
                gpio_set_value(ROW_4_Pin, 1);
            break;
            case 2:
                gpio_set_value(ROW_1_Pin, 1);
                gpio_set_value(ROW_2_Pin, 1);
                gpio_set_value(ROW_3_Pin, 0);
                gpio_set_value(ROW_4_Pin, 1);
            break;
                
            case 3:
                gpio_set_value(ROW_1_Pin, 1);
                gpio_set_value(ROW_2_Pin, 1);
                gpio_set_value(ROW_3_Pin, 1);
                gpio_set_value(ROW_4_Pin, 0);
            break;
        }
    // Read current rows
        if(gpio_get_value(gpioButton_COL[gpio_pin]) == 0)
        { 

            pr_info("mang la , %d,%d\n",i,gpio_pin);
            gpio_set_value(ROW_1_Pin, 1);
            gpio_set_value(ROW_2_Pin, 1);
            gpio_set_value(ROW_3_Pin, 1);
            gpio_set_value(ROW_4_Pin, 1);
            return keys[i][gpio_pin];
        }
        }
    pr_info("mang la , %d,%d\n",i,gpio_pin);
    gpio_set_value(ROW_1_Pin, 1);
    gpio_set_value(ROW_2_Pin, 1);
    gpio_set_value(ROW_3_Pin, 1);
    gpio_set_value(ROW_4_Pin, 1);
    return 0; // No key pressed
}
int charToInt(char c) {
    return c - '0';
}
static irq_handler_t button_irq_handler(unsigned int irq, void *dev_id,
					struct pt_regs *regs)
{
	pr_info("GPIO_TEST %d: Interrupt! \n:", irq);
    // pr_info("val Interrupt %d\n",gpio_get_value(gpioButton_COL[0]));
    // pr_info("val Interrupt %d\n",gpio_get_value(gpioButton_COL[1]));
    // pr_info("val Interrupt %d\n",gpio_get_value(gpioButton_COL[2]));
    // pr_info("val Interrupt %d\n",gpio_get_value(gpioButton_COL[3]));

	if (irq == irqNumber[0]) {
        key_pressed=keypad_scan(0);
        button_status=charToInt(key_pressed);
        pr_info("button %d", button_status);
		send_sig_to_user(button_status);
	} else if (irq == irqNumber[1]) {
        key_pressed=keypad_scan(1);
        button_status=charToInt(key_pressed);
        pr_info("button %d", button_status);
		send_sig_to_user(button_status);
	} else if (irq == irqNumber[2]) {
        key_pressed=keypad_scan(2);
        button_status=charToInt(key_pressed);
        pr_info("button %d", button_status);
		send_sig_to_user(button_status);
	} else if (irq == irqNumber[3]) {
        key_pressed=keypad_scan(3);
        button_status=charToInt(key_pressed);
        pr_info("button %d", button_status);
		send_sig_to_user(button_status);
	}
	return (irq_handler_t) IRQ_HANDLED;
}

module_init(button_init);
module_exit(button_exit);

MODULE_LICENSE("GPL");