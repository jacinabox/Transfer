#include "stdafx.h"
#include <string>
#include <list>
#include <utility>
#ifdef WIN32

#include <windows.h>

struct CALCULATOR_STATE {
	double n;
	WORD op;
	std::string s;
	char s_display[80];
};

static CALCULATOR_STATE numeric(CALCULATOR_STATE cs, MSG msg) {
	WORD op = cs.op;
	WORD vk = msg.wParam;
	double m =  atof(cs.s.c_str());
	//ToAscii(msg.wParam, 0, reinterpret_cast<const BYTE*>(msg.lParam), &ch, 0);

	switch (vk) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case VK_DECIMAL:
		cs.s.push_back(vk);
		strncpy(cs.s_display, cs.s.c_str(), sizeof(cs.s_display));
		break;
	case VK_ADD:
	case VK_SUBTRACT:
	case VK_MULTIPLY:
	case VK_DIVIDE:
		//snprintf(cs.s_display, sizeof(cs.s_display) ,"%f", cs.n);
		cs.op = vk;
		//Because we're being cute let's implement this with a fallthrough.
	case VK_RETURN:
		//First execute the op that is there.
		switch (op) {
		case VK_ADD:
			cs.n += m;

			break;
		case VK_SUBTRACT:
			cs.n -= m;

			break;
		case VK_MULTIPLY:
			cs.n *= m;

			break;
		case VK_DIVIDE:
			cs.n /= m;

			break;
		}

		//Clear the text buffer.
		cs.s.clear();
		snprintf(cs.s_display, sizeof(cs.s_display), "%f", cs.n);
		break;
			

			
		
			

			
		
			

			
		
		//cs.op = ch;
		//break;
	case 'c':
	case 'C':
		cs.n = 0;
		cs.op = VK_ADD;
		cs.s.clear();
		cs.s_display[0] = '\0';
		break;
	}
	return cs;

}
/*const static char* control_array[] = { "7","8","9","+","-","4","5","6","*","/","1","2","3",".","C" };

Transfer<Nothing, Nothing>& control_construct(HWND hwnd) {
	std::list<Transfer<Nothing, Nothing>*> ls;

	for (unsigned ii = 0;ii < sizeof(control_array) / sizeof(control_array[0]);++ii) {

		ls.push_back(&(create_control2("Button", { 0, 0, 50, 50 }, control_array[ii], 101+ii, hwnd) >>
			map(null_sink2<HWND>)));

	}

	return merge(ls);
}*/

static std::string cs_to_string(CALCULATOR_STATE cs) {
	return std::string(cs.s_display);
}

static HWND hwnd;
static MSG helper_button(MSG msg) {
	char s[2];

	GetWindowTextA(reinterpret_cast<HWND>(msg.lParam), s, 2);
	msg.hwnd = hwnd;
	msg.message = WM_KEYDOWN;
	//Hand translate characters to virtual keys (ugh, ever heard of keyboard
	//accelerators?).
	switch (s[0]) {
	case '+':
		msg.wParam = VK_ADD;
		break;
	case '-':
		msg.wParam = VK_SUBTRACT;
		break;
	case '*':
		msg.wParam = VK_MULTIPLY;
		break;
	case '/':
		msg.wParam = VK_DIVIDE;
		break;
	case '.':
		msg.wParam = VK_DECIMAL;
		break;
	default:
		//Most of the relevant virtual key codes are identical as the corresponding
		//character codes (because luck).
		msg.wParam = s[0];
	}
	//msg.wParam = s[0];
	msg.lParam = 0;

	return msg;
}

void calculator_sample() {
	hwnd = create_frame_window(_T("Calculator Sample"), NULL, NULL);
	//SetWindowPos(hwnd, 0, 0, 0, 640, 480, SWP_NOMOVE);
	CALCULATOR_STATE cs_init = { 0 };
	RECT rect = { 0, 0, 350, 480 };

	cs_init.op = VK_ADD;

	//TODO work in progress
	Transfer<Nothing, Nothing>& transfer = win32_source() >>
		(map(null_sink2<MSG>) >>
			create_control2("Static", { 0, 0, 300, 20 }, "0", 100, hwnd) >>
			map(null_sink2<HWND>) |

			map(null_sink2<MSG>) >>
			create_control2("Button", { 0, 0, 50, 50 }, "7", 101, hwnd) >>
			map(null_sink2<HWND>) |

			map(null_sink2<MSG>) >>
			create_control2("Button", { 0, 0, 50, 50 }, "8", 102, hwnd) >>
			map(null_sink2<HWND>) |

			map(null_sink2<MSG>) >>
			create_control2("Button", { 0, 0, 50, 50 }, "9", 103, hwnd) >>
			map(null_sink2<HWND>) |

			map(null_sink2<MSG>) >>
			create_control2("Button", { 0, 0, 50, 50 }, "+", 104, hwnd) >>
			map(null_sink2<HWND>) |

			map(null_sink2<MSG>) >>
			create_control2("Button", { 0, 0, 50, 50 }, "-", 105, hwnd) >>
			map(null_sink2<HWND>) |


			map(null_sink2<MSG>) >>
			create_control2("Button", { 0, 0, 50, 50 }, "4", 106, hwnd) >>
			map(null_sink2<HWND>) |

			map(null_sink2<MSG>) >>
			create_control2("Button", { 0, 0, 50, 50 }, "5", 107, hwnd) >>
			map(null_sink2<HWND>) |

			map(null_sink2<MSG>) >>
			create_control2("Button", { 0, 0, 50, 50 }, "6", 108, hwnd) >>
			map(null_sink2<HWND>) |

			map(null_sink2<MSG>) >>
			create_control2("Button", { 0, 0, 50, 50 }, "*", 109, hwnd) >>
			map(null_sink2<HWND>) |

			map(null_sink2<MSG>) >>
			create_control2("Button", { 0, 0, 50, 50 }, "/", 110, hwnd) >>
			map(null_sink2<HWND>) |


			map(null_sink2<MSG>) >>
			create_control2("Button", { 0, 0, 50, 50 }, "1", 111, hwnd) >>
			map(null_sink2<HWND>) |

			map(null_sink2<MSG>) >>
			create_control2("Button", { 0, 0, 50, 50 }, "2", 112, hwnd) >>
			map(null_sink2<HWND>) |

			map(null_sink2<MSG>) >>
			create_control2("Button", { 0, 0, 50, 50 }, "3", 113, hwnd) >>
			map(null_sink2<HWND>) |

			map(null_sink2<MSG>) >>
			create_control2("Button", { 0, 0, 50, 50 }, ".", 114, hwnd) >>
			map(null_sink2<HWND>) |

			map(null_sink2<MSG>) >>
			create_control2("Button", { 0, 0, 50, 50 }, "C", 115, hwnd) >>
			map(null_sink2<HWND>) |

			/*	map(null_sink2<MSG>) >>
				create_control2("Button", { 0, 0, 50, 50 }, control_array[1], 102, hwnd) >>
				map(null_sink2<HWND>) |

				map(null_sink2<MSG>) >>
				create_control2("Button", { 0, 0, 50, 50 }, control_array[2], 103, hwnd) >>
				map(null_sink2<HWND>) |*/

		map(std::function<RECT(MSG)>(std::bind(const__<RECT, MSG>, rect, _1))) >>
			resize_window2(hwnd) |

			size_children_according_to_layout(hwnd) |

		filter_code(WM_KEYDOWN) >>
			scanning(cs_init, numeric) >>
			map(cs_to_string) >>
			set_window_text(hwnd, 100) >>
			map(null_sink2<BOOL>) |

		filter_code(WM_COMMAND) >>
			map(helper_button) >>
			post_message() >>
			map(null_sink2<BOOL>));

	Nothing nothing;
	std::function<Nothing()> f0(std::bind(identity__<Nothing>, nothing));

	Transfer<Nothing, Nothing>::transduce_loop(f0, 
		std::auto_ptr<Transfer<Nothing, Nothing> >(&transfer),
		std::function<void(const Nothing&)>(null_sink2<Nothing>));
}

#endif