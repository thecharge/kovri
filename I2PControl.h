#ifndef I2P_CONTROL_H__
#define I2P_CONTROL_H__

#include <inttypes.h>
#include <thread>
#include <memory>
#include <array>
#include <string>
#include <map>
#include <boost/asio.hpp>

namespace i2p
{
namespace client
{
	const size_t I2P_CONTROL_MAX_REQUEST_SIZE = 1024;
	typedef std::array<char, I2P_CONTROL_MAX_REQUEST_SIZE> I2PControlBuffer;		

	const char I2P_CONTROL_PROPERTY_ID[] = "id";
	const char I2P_CONTROL_PROPERTY_METHOD[] = "method";
	const char I2P_CONTROL_PROPERTY_TOKEN[] = "Token";
	const char I2P_CONTROL_PROPERTY_PARAMS[] = "params";

	class I2PControlService
	{
		public:

			I2PControlService (int port);
			~I2PControlService ();

			void Start ();
			void Stop ();

		private:

			void Run ();
			void Accept ();
			void HandleAccept(const boost::system::error_code& ecode, std::shared_ptr<boost::asio::ip::tcp::socket> socket);	
			void ReadRequest (std::shared_ptr<boost::asio::ip::tcp::socket> socket);
			void HandleRequestReceived (const boost::system::error_code& ecode, size_t bytes_transferred, 
				std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<I2PControlBuffer> buf);

		private:

			bool m_IsRunning;
			std::thread * m_Thread;	

			boost::asio::io_service m_Service;
			boost::asio::ip::tcp::acceptor m_Acceptor;	

			typedef void (I2PControlService::*MethodHandler)();
			std::map<std::string, MethodHandler> m_MethodHanders;		
	};
}
}

#endif

