#ifndef _ADS114S08B_H_
#define _ADS114S08B_H_

// Control Commands
#define ADS114S08B_CMD_NOP                          0x00
#define ADS114S08B_CMD_WAKEUP                       0x02
#define ADS114S08B_CMD_POWERDOWN                    0x04
#define ADS114S08B_CMD_RESET                        0x06
#define ADS114S08B_CMD_START                        0x08
#define ADS114S08B_CMD_STOP                         0x0A

// Calibration Commands
#define ADS114S08B_CMD_SYOCAL                       0x16
#define ADS114S08B_CMD_SYGCAL                       0x17
#define ADS114S08B_CMD_SFOCAL                       0x19

// Data Read Command
#define ADS114S08B_CMD_RDATA                        0x12

// Register Read and Write Commands
#define ADS114S08B_CMD_RREG(r)                      (0x20|(0x1F&(r)))
#define ADS114S08B_CMD_WREG(r)                      (0x40|(0x1F&(r)))

#define ADS114S08B_REG_ID                           0x00
#define ADS114S08B_REG_STATUS                       0x01
#define ADS114S08B_REG_INPMUX                       0x02
#define ADS114S08B_REG_PGA                          0x03
#define ADS114S08B_REG_DATARATE                     0x04
#define ADS114S08B_REG_REF                          0x05
#define ADS114S08B_REG_IDACMAG                      0x06
#define ADS114S08B_REG_IDACMUX                      0x07
#define ADS114S08B_REG_VBIAS                        0x08
#define ADS114S08B_REG_SYS                          0x09
#define ADS114S08B_REG_RESERVED1                    0x0A
#define ADS114S08B_REG_OFCAL0                       0x0B
#define ADS114S08B_REG_OFCAL1                       0x0C
#define ADS114S08B_REG_RESERVED2                    0x0D
#define ADS114S08B_REG_FSCAL0                       0x0E
#define ADS114S08B_REG_FSCAL1                       0x0F
#define ADS114S08B_REG_GPIODAT                      0x10
#define ADS114S08B_REG_GPIOCON                      0x11
#define ADS114S08B_NUM_REGS                         0x12

// STATUS, OFCAL0, OFCAL1, FSCAL0, FSCAL1, GPIODAT all volatile
#define ADS114S08B_VOLATILE_REG_MASK                0b011101100000000010UL

#define ADS114S08B_REG_ID_DEV_ID_MASK               0x07

#define ADS114S08B_REG_STATUS_FL_POR                0x80
#define ADS114S08B_REG_STATUS_RDY_N                 0x40
#define ADS114S08B_REG_STATUS_FL_REF                0x41

#define ADS114S08B_AIN0                             0x0
#define ADS114S08B_AIN1                             0x1
#define ADS114S08B_AIN2                             0x2
#define ADS114S08B_AIN3                             0x3
#define ADS114S08B_AIN4                             0x4
#define ADS114S08B_AIN5                             0x5
#define ADS114S08B_AIN6                             0x6
#define ADS114S08B_AIN7                             0x7
#define ADS114S08B_AIN8_GPIO0                       0x8
#define ADS114S08B_AIN8                             ADS114S08B_AIN8_GPIO0
#define ADS114S08B_GPIO0                            ADS114S08B_AIN8_GPIO0
#define ADS114S08B_AIN9_GPIO1                       0x9
#define ADS114S08B_AIN9                             ADS114S08B_AIN9_GPIO1
#define ADS114S08B_GPIO1                            ADS114S08B_AIN9_GPIO1
#define ADS114S08B_AIN10_GPIO2                      0xA
#define ADS114S08B_AIN10                            ADS114S08B_AIN10_GPIO2
#define ADS114S08B_GPIO2                            ADS114S08B_AIN10_GPIO2
#define ADS114S08B_AIN11_GPIO3                      0xB
#define ADS114S08B_AIN11                            ADS114S08B_AIN11_GPIO3
#define ADS114S08B_GPIO3                            ADS114S08B_AIN11_GPIO3
#define ADS114S08B_AINCOM                           0xC
#define ADS114S08B_OFF                              0xF

#define ADS114S08B_REG_INPMUX_MUX(chp,chn)          ((((chp)&0xF)<<4)|((chn)&0xF))

#define ADS114S08B_REG_PGA_OFF                      0x00
#define ADS114S08B_REG_PGA_EN                       0x08
#define ADS114S08B_REG_PGA_GAIN1                    0x00
#define ADS114S08B_REG_PGA_GAIN2                    0x01
#define ADS114S08B_REG_PGA_GAIN4                    0x02
#define ADS114S08B_REG_PGA_GAIN8                    0x03
#define ADS114S08B_REG_PGA_GAIN16                   0x04
#define ADS114S08B_REG_PGA_GAIN32                   0x05
#define ADS114S08B_REG_PGA_GAIN64                   0x06
#define ADS114S08B_REG_PGA_GAIN128                  0x07

#define ADS114S08B_REG_DATARATE_CLK_INTERNAL        0x00
#define ADS114S08B_REG_DATARATE_CLK_EXTERNAL        0x40
#define ADS114S08B_REG_DATARATE_MODE_CONTINOUS      0x00
#define ADS114S08B_REG_DATARATE_MODE_SINGLE_SHOT    0x20
#define ADS114S08B_REG_DATARATE_RESERVED            0x10
#define ADS114S08B_REG_DATARATE_DR_2P5              0x00
#define ADS114S08B_REG_DATARATE_DR_5                0x01
#define ADS114S08B_REG_DATARATE_DR_10               0x02
#define ADS114S08B_REG_DATARATE_DR_16P5             0x03
#define ADS114S08B_REG_DATARATE_DR_20               0x04
#define ADS114S08B_REG_DATARATE_DR_50               0x05
#define ADS114S08B_REG_DATARATE_DR_60               0x06
#define ADS114S08B_REG_DATARATE_DR_100              0x07
#define ADS114S08B_REG_DATARATE_DR_200              0x08
#define ADS114S08B_REG_DATARATE_DR_400              0x09
#define ADS114S08B_REG_DATARATE_DR_800              0x0A
#define ADS114S08B_REG_DATARATE_DR_1000             0x0B
#define ADS114S08B_REG_DATARATE_DR_2000             0x0C
#define ADS114S08B_REG_DATARATE_DR_4000             0x0D

#define ADS114S08B_REG_REF_REFSEL_REF0              0x00

#define ADS114S08B_REG_REF_REFSEL_FL_REF_EN         0x40
#define ADS114S08B_REG_REF_REFSEL_REFP_BUF_N        0x20
#define ADS114S08B_REG_REF_REFSEL_REFN_BUF_N        0x10

#define ADS114S08B_REG_REF_REFSEL_REF1              0x04
#define ADS114S08B_REG_REF_REFSEL_INT_2V5           0x08

#define ADS114S08B_REG_REF_REFCON_OFF               0x00
#define ADS114S08B_REG_REF_REFCON_ON_PWRDN          0x01
#define ADS114S08B_REG_REF_REFCON_ON                0x02

#define ADS114S08B_REG_IDACMAG_OFF                  0x00
#define ADS114S08B_REG_IDACMAG_10UA                 0x01
#define ADS114S08B_REG_IDACMAG_50UA                 0x02
#define ADS114S08B_REG_IDACMAG_100UA                0x03
#define ADS114S08B_REG_IDACMAG_250UA                0x04
#define ADS114S08B_REG_IDACMAG_500UA                0x05
#define ADS114S08B_REG_IDACMAG_750UA                0x06
#define ADS114S08B_REG_IDACMAG_1000UA               0x07
#define ADS114S08B_REG_IDACMAG_1500UA               0x08
#define ADS114S08B_REG_IDACMAG_2000UA               0x09

#define ADS114S08B_REG_IDACMUX_MUX(a,b)             ((((a)&0xF)<<4)|((b)&0xF))
#define ADS114S08B_REG_IDACMUX_DISCONNECTED         0xFF

#define ADS114S08B_REG_VBIAS_OFF                    0x00
#define ADS114S08B_REG_VBIAS_AIN0                   0x01
#define ADS114S08B_REG_VBIAS_AIN1                   0x02
#define ADS114S08B_REG_VBIAS_AIN2                   0x04
#define ADS114S08B_REG_VBIAS_AIN3                   0x08
#define ADS114S08B_REG_VBIAS_AIN4                   0x10
#define ADS114S08B_REG_VBIAS_AIN5                   0x20
#define ADS114S08B_REG_VBIAS_AINC                   0x40

#define ADS114S08B_REG_SYS_SYS_MON_DISABLE          0x00
#define ADS114S08B_REG_SYS_SYS_MON_AVDD_AVSS_DIV2   0x20
#define ADS114S08B_REG_SYS_SYS_MON_TEMP             0x40
#define ADS114S08B_REG_SYS_SYS_MON_AVDD_AVSS_DIV4   0x60
#define ADS114S08B_REG_SYS_SYS_MON_DVDD_DIV4        0x80
#define ADS114S08B_REG_SYS_SYS_MON_BURNOUT_0P2UA    0xA0
#define ADS114S08B_REG_SYS_SYS_MON_BURNOUT_1UA      0xC0
#define ADS114S08B_REG_SYS_SYS_MON_BURNOUT_19UA     0xE0

#define ADS114S08B_REG_SYS_CAL_SAMP_1               0x00
#define ADS114S08B_REG_SYS_CAL_SAMP_4               0x08
#define ADS114S08B_REG_SYS_CAL_SAMP_8               0x10
#define ADS114S08B_REG_SYS_CAL_SAMP_16              0x18

#define ADS114S08B_REG_SYS_TIMEOUT_DISABLED         0x00
#define ADS114S08B_REG_SYS_TIMEOUT_DISABLED         0x04

#endif // _ADS114S08B_H_
