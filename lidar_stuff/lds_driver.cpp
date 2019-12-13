#include "lds_driver.h"

#include <ctime>

namespace lds {
  LFCDLaser::LFCDLaser(const std::string & port, uint32_t baud_rate, boost::asio::io_service & io,
    const std::string & port_1, uint32_t baud_rate_1, boost::asio::io_service & io1): port_(port), baud_rate_(baud_rate), shutting_down_(false), serial_(io, port_), port1(port_1), baud_rate1(baud_rate_1), serial1(io1, port1) {
    serial_.set_option(boost::asio::serial_port_base::baud_rate(baud_rate_));
    serial1.set_option(boost::asio::serial_port_base::baud_rate(baud_rate1));

    boost::asio::write(serial_, boost::asio::buffer("b", 1)); // start motor

  }

  LFCDLaser::~LFCDLaser() {
    boost::asio::write(serial_, boost::asio::buffer("e", 1)); // stop motor
  }

  void LFCDLaser::poll() {
    instruction = "N";
    uint8_t temp_char;
    uint8_t start_count = 0;
    bool got_scan = false;
    boost::array < uint8_t, 2520 > raw_bytes;
    uint8_t good_sets = 0;
    uint32_t motor_speed = 0;
    rpms = 0;
    int index;
    int angle = 0;
    double sumF1 = 0;
    double sumB1 = 0;
    double sumF2 = 0;
    double sumB2 = 0;
    double sumL1 = 0;
    double sumR1 = 0;
    double sumL2 = 0;
    double sumR2 = 0;
    double value_maxF1 = 0.35;
    double value_maxF2 = 0.35;
    double value_maxB1 = 0.5;
    double value_maxB2 = 0.4;
    double value_maxL1 = 0.2;
    double value_maxL2 = 0.3;
    double value_maxR1 = 0.3;
    double value_maxR2 = 0.3;
    int counterF1 = 0;
    int counterB1 = 0;
    int counterF2 = 0;
    int counterB2 = 0;
    int counterL1 = 0;
    int counterR1 = 0;
    int counterL2 = 0;
    int counterR2 = 0;

    while (!shutting_down_ && !got_scan) {
      // Wait until first data sync of frame: 0xFA, 0xA0
      boost::asio::read(serial_, boost::asio::buffer( & raw_bytes[start_count], 1));

      if (start_count == 0) {
        if (raw_bytes[start_count] == 0xFA) {
          start_count = 1;
        }
      } else if (start_count == 1) {
        if (raw_bytes[start_count] == 0xA0) {
          start_count = 0;

          // Now that entire start sequence has been found, read in the rest of the message
          got_scan = true;

          boost::asio::read(serial_, boost::asio::buffer( & raw_bytes[2], 2518));

          //read data in sets of 6
          for (uint16_t i = 0; i < raw_bytes.size(); i = i + 42) {
            if (raw_bytes[i] == 0xFA && raw_bytes[i + 1] == (0xA0 + i / 42)) //&& CRC check
            {
              good_sets++;
              motor_speed += (raw_bytes[i + 3] << 8) + raw_bytes[i + 2]; //accumulate count for avg. time increment
              rpms = (raw_bytes[i + 3] << 8 | raw_bytes[i + 2]) / 10;

              for (uint16_t j = i + 4; j < i + 40; j = j + 6) {
                index = 6 * (i / 42) + (j - 4 - i) / 6;

                // Four bytes per reading
                uint8_t byte0 = raw_bytes[j];
                uint8_t byte1 = raw_bytes[j + 1];
                uint8_t byte2 = raw_bytes[j + 2];
                uint8_t byte3 = raw_bytes[j + 3];

                // Remaining bits are the range in mm
                uint16_t intensity = (byte1 << 8) + byte0;

                // Last two bytes represent the uncertanty or intensity, might also be pixel area of target...
                uint16_t range = (byte3 << 8) + byte2;

                angle = 359 - index;
                double temp_range = (range / 1000.0);
                //decrementing angle
                if (angle <= 25 || angle >= 325) {
                  if (temp_range > 0.0) {
                    sumB1 = sumB1 + temp_range;
                    counterB1 = counterB1 + 1;
                  }
                  if (angle == 0) {
                    double avgB1 = sumB1 / counterB1;
                    if (avgB1 < value_maxB1) {
                      printf("back1=%f,", avgB1);
                      printf("\n");
                      instruction = "B";
                    }
                    sumB1 = 0.0;
                    avgB1 = 0.0;
                    counterB1 = 0;
                  }
                }
                if ((angle >= 25 && angle <= 45) || (angle >= 315 && angle <= 335)) {
                  if (temp_range > 0.0) {
                    sumB2 = sumB2 + temp_range;
                    counterB2 = counterB2 + 1;
                  }
                  if (angle == 25) {
                    double avgB2 = sumB2 / counterB2;
                    if (avgB2 < value_maxB2) {
                      printf("back2=%f,", avgB2);
                      printf("\n");
                      instruction = "B";
                    }
                    sumB2 = 0.0;
                    avgB2 = 0.0;
                    counterB2 = 0;
                  }
                }
                if (angle >= 58 && angle <= 80) {
                  if (temp_range > 0.0) {
                    sumR1 = sumR1 + temp_range;
                    counterR1 = counterR1 + 1;
                  }
                  if (angle == 58) {
                    double avgR1 = sumR1 / counterR1;
                    if (avgR1 < value_maxR1) {
                      printf("right1=%f,", avgR1);
                      printf("\n");
                      instruction = "R";
                    }
                    sumR1 = 0.0;
                    avgR1 = 0.0;
                    counterR1 = 0;
                  }
                }
                if (angle >= 80 && angle <= 122) {
                  if (temp_range > 0.0) {
                    sumR2 = sumR2 + temp_range;
                    counterR2 = counterR2 + 1;
                  }
                  if (angle == 80) {
                    double avgR2 = sumR2 / counterR2;
                    if (avgR2 < value_maxR2) {
                      printf("right2=%f,", avgR2);
                      printf("\n");
                      instruction = "R";
                    }
                    sumR2 = 0.0;
                    avgR2 = 0.0;
                    counterR2 = 0;
                  }
                }
                if (angle >= 155 && angle <= 200) {
                  if (temp_range > 0.0) {
                    sumF1 = sumF1 + temp_range;
                    counterF1 = counterF1 + 1;
                  }
                  if (angle == 155) {
                    double avgF1 = sumF1 / counterF1;
                    if (avgF1 < value_maxF1) {
                      printf("front1=%f,", avgF1);
                      printf("\n");
                      instruction = "F";
                    }
                    sumF1 = 0.0;
                    avgF1 = 0.0;
                    counterF1 = 0;
                  }
                }
                if ((angle >= 135 && angle <= 155) || (angle >= 200 && angle <= 225)) {
                  if (temp_range > 0.0) {
                    sumF2 = sumF2 + temp_range;
                    counterF2 = counterF2 + 1;
                  }
                  if (angle == 135) {
                    double avgF2 = sumF2 / counterF2;
                    if (avgF2 < value_maxF2) {
                      printf("front2=%f,", avgF2);
                      printf("\n");
                      instruction = "F";
                    }
                    sumF2 = 0.0;
                    avgF2 = 0.0;
                    counterF2 = 0;
                  }
                }
                if (angle >= 238 && angle <= 260) {
                  if (temp_range > 0.0) {
                    sumL1 = sumL1 + temp_range;
                    counterL1 = counterL1 + 1;
                  }
                  if (angle == 238) {
                    double avgL1 = sumL1 / counterL1;
                    if (avgL1 < value_maxL1) {
                      printf("left1=%f,", avgL1);
                      printf("\n");
                      instruction = "L";
                    }
                    sumL1 = 0.0;
                    avgL1 = 0.0;
                    counterL1 = 0;
                  }
                }
                if (angle >= 260 && angle <= 302) {
                  if (temp_range > 0.0) {
                    sumL2 = sumL2 + temp_range;
                    counterL2 = counterL2 + 1;
                  }
                  if (angle == 260) {
                    double avgL2 = sumL2 / counterL2;
                    if (avgL2 < value_maxL2) {
                      printf("left2=%f,", avgL2);
                      printf("\n");
                      instruction = "L";
                    }
                    sumL2 = 0.0;
                    avgL2 = 0.0;
                    counterL2 = 0;
                  }
                }
                if(!(instruction == "N"))
                {
                  printf("%c\n", instruction[0]);
                  boost::asio::write(serial1, boost::asio::buffer(instruction, 1));
                  instruction = "N";
                }
              }
            }
          }
        } else {
          start_count = 0;
        }
      }
    }
  }
}

int main(int argc, char ** argv) {

  std::string port;
  int baud_rate;

  std::string port1;
  int baud_rate1;

  uint16_t rpms;
  port = "/dev/ttyUSB0";
  baud_rate = 230400;

  port1 = "/dev/ttyACM0";
  baud_rate1 = 4800;

  boost::asio::io_service io;
  boost::asio::io_service io1;

  try {
    lds::LFCDLaser laser(port, baud_rate, io, port1, baud_rate1, io1);

    while (1) {
      laser.poll();
    }
    laser.close();

    return 0;
  } catch (boost::system::system_error ex) {
    printf("An exception was thrown: %s", ex.what());
    return -1;
  }
}
