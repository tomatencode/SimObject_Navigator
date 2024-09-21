#include <cstdio>
#include<fstream>
#include "mainwindow.h"
#include <chrono>
#include <sstream>
#include<string>
#include <iostream>

#include "Eval_functional_model_net.h"

using namespace std::literals::string_literals; // allow suffix ""s for std::string literal

Eval_functional_model_net::Eval_functional_model_net() :
	command_line_label("Command Line:"),
	log_output_label("Log Output:"),
	error_message_label("Error Message:")
{
	setLayout(&layout);

	sim_obj.setText("<command line>");
	sim_obj.setReadOnly(true);
	sim_obj_s.setWidget(&sim_obj);
	sim_obj_s.setWidgetResizable(true);
	//command_line_s.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	sim_obj.setWordWrapMode(QTextOption::NoWrap);
	sim_obj_s.setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));

	layout.addWidget(&get_SimObj, 0, 0);
	layout.addWidget(&sim_obj_s, 0, 1);

	log_output_s.setWidget(&log_output);
	log_output_s.setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));

	layout.addWidget(&log_output_s, 1, 1);

	get_SimObj.setText("Get SimObj");

	auto success = connect(&get_SimObj, SIGNAL(clicked()), this, SLOT(get_demo_SimulationObject()));
	assert(success);
}

std::string Eval_functional_model_net::exec(std::string sim_obj_name)
{
	get_demo_SimulationObject();
	return "";
}

static constexpr bool DEBUG_MSG_SWITCH = false;

#ifdef __EMSCRIPTEN__

// https://gist.github.com/nus/564e9e57e4c107faa1a45b8332c265b9

/*
Could you show me an example how to use emscripten websockets?
PERPLEXITY
Here is an example of how to use Emscripten WebSockets to establish a connection from C++ code:
First, include the websocket.h header file from the Emscripten SDK.
*/
#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>

/*
Next, create a WebSocket object using the emscripten_websocket_new function and specify the URL of the WebSocket server.
c++


*/

namespace {
	EM_BOOL onopen(int eventType, const EmscriptenWebSocketOpenEvent* websocketEvent, void* userData) {
        message("onopen");

		EMSCRIPTEN_RESULT result;
		result = emscripten_websocket_send_utf8_text(websocketEvent->socket, "hoge");
		if (result) {
            message("Failed to emscripten_websocket_send_utf8_text(): "s + std::to_string(result));
		}
		return EM_TRUE;
	}
	EM_BOOL onerror(int eventType, const EmscriptenWebSocketErrorEvent* websocketEvent, void* userData) {
        message("onerror");

		return EM_TRUE;
	}
	EM_BOOL onclose(int eventType, const EmscriptenWebSocketCloseEvent* websocketEvent, void* userData) {
        message("onclose");

		return EM_TRUE;
	}
	EM_BOOL onmessage(int eventType, const EmscriptenWebSocketMessageEvent* websocketEvent, void* userData) {
        message("onmessage");
		if (websocketEvent->isText) {
			// For only ascii chars.
            message("message: \n"s + std::string((const char*)websocketEvent->data));
		}
        else
        {
            message("message: Non -Tex result");

        }
		EMSCRIPTEN_RESULT result;
		result = emscripten_websocket_close(websocketEvent->socket, 1000, "no reason");
		if (result) {
            message("Failed to emscripten_websocket_close(): "s + std::to_string(result));
        }
		return EM_TRUE;
	}
}

void Eval_functional_model_net::get_demo_SimulationObject()
{
    if (not emscripten_websocket_is_supported()) {
		message("Websockets not supported");
		return;
	}
	EmscriptenWebSocketCreateAttributes ws_attrs = {
        "wss://127.0.0.1:5000",
		NULL,
		EM_TRUE
	};

    message("Using emscripten_websocket_new:");
    EMSCRIPTEN_WEBSOCKET_T ws = emscripten_websocket_new(&ws_attrs);
	// Register a callback function to handle WebSocket events using the emscripten_websocket_set_onmessage, emscripten_websocket_set_onopen, emscripten_websocket_set_onerror, and emscripten_websocket_set_onclose functions.

    emscripten_websocket_set_onopen_callback(ws, static_cast<void*>(this), onopen);
    emscripten_websocket_set_onerror_callback(ws, static_cast<void*>(this), onerror);
    emscripten_websocket_set_onclose_callback(ws, static_cast<void*>(this), onclose);
    emscripten_websocket_set_onmessage_callback(ws, static_cast<void*>(this), onmessage);

    message("Using emscripten_websocket_send_utf8_text:");


    // Create an HTTP GET request
    std::string request = "GET /get_test_sim_object HTTP/1.1\r\n"s;
    request += "Host: "s + url + "\r\n"s;
    request += "Connection: close\r\n\r\n"s;
    auto result = emscripten_websocket_send_utf8_text(ws, request.c_str());

    message ("Result emscripten_websocket_send_utf8_text: "s + std::to_string(result));

	/*
    // Receive the response from the server
      char* data = nullptr;
      int data_len = 0;
      int flags = 0;
      emscripten_websocket_recv_utf8(ws, &data, &flags);
	  */
      // Print the response to the console
      //std::string response(data, data_len);
      //message("Response: "s + response);

      // Close the WebSocket connection
      emscripten_websocket_close(ws, 1000, "WebSocket closed normally");
}

#else
#include <asio.hpp>

void Eval_functional_model_net::get_demo_SimulationObject()
{
	auto DEBUG_MSG = [this](std::string msg)->void
	{
		if (DEBUG_MSG_SWITCH)
		{
			//message(msg);
			log_output_ += msg + "\n"s;
			log_output.setText(log_output_.c_str());
		}
	};

	try {
		DEBUG_MSG("0");
		DEBUG_MSG("0.0");

		asio::io_context io_context;
		DEBUG_MSG("0.1");

		asio::ip::tcp::resolver resolver(io_context);
		DEBUG_MSG("0.2");
		asio::ip::tcp::socket socket(io_context);

		DEBUG_MSG("0.3");

		// Resolve the host name and port number to an iterator that can be used to connect to the server.
		asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(url.c_str(), std::to_string(port).c_str());

		// Connect the socket to the server.
		asio::connect(socket, endpoints);

		/* this is how parameters are added:
		std::string params = "key1=" + boost::asio::detail::url_encode("value1")
			+ "&key2=" + boost::asio::detail::url_encode("value2");
		std::string request = "GET " + path + "?" + params + " HTTP/1.1\r\n"
					  + "Host: " + hostname + "\r\n"
					  + "Connection: close\r\n\r\n";
		*/
		// Create an HTTP GET request
		std::string request = "GET /get_test_sim_object HTTP/1.1\r\n"s;
		request += "Host: "s + url + "\r\n"s;
		request += "Connection: close\r\n\r\n"s;

		DEBUG_MSG("1");

		// Send the HTTP request to the remote host
		asio::write(socket, asio::buffer(request));

		DEBUG_MSG("2");

		// Receive the HTTP response from the remote host
		std::string response;
		std::size_t bytes_transferred;
		do {
			char buf[1024];
			try {
				bytes_transferred = socket.read_some(asio::buffer(buf, sizeof(buf)));
			}
			catch (std::exception const& e) {
				if (e.what() == std::string("read_some: End of file")) {
					break;  // End of file exception caught, exit loop
				}
				else {
					throw;  // Rethrow other exceptions
				}
			}
			response.append(buf, bytes_transferred);
		} while (bytes_transferred > 0);

		DEBUG_MSG("2");
		// Print the HTTP response to the console
		sim_obj_ = response;
		sim_obj.setText(sim_obj_.c_str());
	}
	catch (std::exception& exc)
	{
		DEBUG_MSG("Exc"s + exc.what());

		message(exc.what());
	}
}
#endif
