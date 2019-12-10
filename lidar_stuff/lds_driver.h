#include <string>
#include <boost/asio.hpp>
#include <boost/array.hpp>

namespace lds
{
class LFCDLaser
{
public:
	uint16_t rpms; ///< @brief RPMS derived from the rpm bytes in an LFCD packet

	LFCDLaser(const std::string& port, uint32_t baud_rate, boost::asio::io_service& io, const std::string& port1, uint32_t baud_rate1, boost::asio::io_service & io1);

	~LFCDLaser();

	void poll();

	void close() { shutting_down_ = true; }

private:
	std::string port_; ///< @brief The serial port the driver is attached to
	uint32_t baud_rate_; ///< @brief The baud rate for the serial connection
	bool shutting_down_; ///< @brief Flag for whether the driver is supposed to be shutting down or not
	boost::asio::serial_port serial_; ///< @brief Actual serial port object for reading/writing to the LFCD Laser Scanner
	uint16_t motor_speed_; ///< @brief current motor speed as reported by the LFCD.

  std::string port1; ///< @brief The serial port the driver is attached to
  uint32_t baud_rate1; ///< @brief The baud rate for the serial connection
  boost::asio::serial_port serial1;

  std::string instruction;
  int a;
};
}
