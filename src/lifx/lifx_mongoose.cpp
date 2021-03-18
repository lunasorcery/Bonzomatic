extern "C" {
#include "mongoose.h"
}
#include <stdio.h>
#include <string>

#include "Renderer.h"

namespace LIFX {

	static void ev_handler(struct mg_connection* nc, int ev, void* ev_data, void* fn_data) {
		(void)nc;

		switch (ev) {
			case MG_EV_CONNECT: {
				int status = *((int *)ev_data);
				if (status != 0) {
					printf("-- Connection error: %d\n", status);
				}
				break;
			}
			case MG_EV_READ: {
				break;
			}
			case MG_EV_CLOSE: {
				break;
			}
		}
	}

	struct rgb_t {
		float r,g,b;
	};
	struct hsb_t {
		uint16_t h,s,b;
	};

	struct mg_mgr mgr;
	struct mg_connection *nc;

	int nWidth, nHeight;
	uint8_t* pixelBuffer;

	bool Open(RENDERER_SETTINGS const& settings) {
		nWidth = settings.nWidth;
		nHeight = settings.nHeight;
		pixelBuffer = new uint8_t[settings.nWidth * settings.nHeight * 4];
		std::string const ServerURL = "udp://192.168.1.138:56700";
		printf("Try to connect to %s\n", ServerURL.c_str());
		mg_mgr_init(&mgr);
		nc = mg_connect(&mgr, ServerURL.c_str(), ev_handler, NULL);
		if (nc == NULL) {
			fprintf(stderr, "Invalid address\n");
			return false;
		}
		return true;
	}

	hsb_t RGB2HSB(rgb_t rgb)
	{
		hsb_t hsb;

		float const rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
		float const rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

		hsb.b = rgbMax * 0xffff;
		if (hsb.b == 0)
		{
			hsb.h = 0;
			hsb.s = 0;
			return hsb;
		}

		hsb.s = 0xffff * ((rgbMax - rgbMin) / rgbMax);
		if (hsb.s == 0)
		{
			hsb.h = 0;
			return hsb;
		}

		if (rgbMax == rgb.r)
			hsb.h = 0x0000 + 0x2AAA * (rgb.g - rgb.b) / (rgbMax - rgbMin);
		else if (rgbMax == rgb.g)
			hsb.h = 0x5555 + 0x2AAA * (rgb.b - rgb.r) / (rgbMax - rgbMin);
		else
			hsb.h = 0xAAAA + 0x2AAA * (rgb.r - rgb.g) / (rgbMax - rgbMin);

		return hsb;

	}

	void UpdateColor()
	{
		static int frame = 0;
		frame++;

		if ((frame % 2) != 0)
			return;

		if (!pixelBuffer)
			return;

		if (!Renderer::GrabFrame( pixelBuffer ))
			return;

		float r = 0;
		float g = 0;
		float b = 0;
		float w = 0;
		int const stride = nWidth*4;
		for (int y = 0; y < nHeight; ++y) {
			for (int x = 0; x < nWidth; ++x) {
				r += pixelBuffer[y*stride+x*4+0]/255.0f;
				g += pixelBuffer[y*stride+x*4+1]/255.0f;
				b += pixelBuffer[y*stride+x*4+2]/255.0f;
				w++;
			}
		}
		r/=w;
		g/=w;
		b/=w;
		rgb_t const rgb = {r,g,b};
		hsb_t const hsb = RGB2HSB(rgb);

		static uint8_t seq = 0;
		seq++;

		printf("Setting bulb color\n");
		printf("RGB: %0.3f, %0.3f, %0.3f\n", r, g, b);
		printf("HSB: %04x, %04x, %04x\n", hsb.h, hsb.s, hsb.b);

		uint8_t packet[] = {
			// frame header
			0x2f, 0x00, // size
			0x00, 0x14, // protocol
			'b', 'z', 'm', 'c', // source

			// frame address
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // target
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // reserved
			0,//0x02, // flags
			seq, // sequence

			// message header
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // reserved
			0x66, 0x00, // message type (SetColor)
			0x00, 0x00, // reserved

			// payload
			0x00, // reserved
			(hsb.h&0xff), (hsb.h>>8), // hue
			(hsb.s&0xff), (hsb.s>>8), // saturation
			(hsb.b&0xff), (hsb.b>>8), // brightness
			0xac, 0x0d, // kelvin
			0x00, 0x00, // transition time
		};
		mg_send(nc, packet, sizeof(packet));
	}

	void Poll() {
		// TODO: should be in another thread?
		mg_mgr_poll(&mgr, 10);
	}

	bool Close() {
		mg_mgr_free(&mgr);
		delete[] pixelBuffer;
		return true;
	}
} // namespace LIFX