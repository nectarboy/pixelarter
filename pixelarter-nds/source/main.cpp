#include <nds.h>

#include <iostream>

#include <gl2d.h>

using namespace std;


volatile int frame = 0;

void Vblank () {

	frame ++;

}

class Painter {

	public:

		// canvas details //

		static const int size = 16; // size x size canvas ya3ne

		static const int area = size * size;

		static const int px = 192 / size;

		int canv [area] = {};

		int colors;

		// painter details //

		int x = 0;

		int y = 0;

		int indx = 0;


		int R = 0;

		int G = 255;

		int B = 0;


		int cursor = RGB15 (255, 255, 255);

		// methods //

		void MovePos (int mx, int my) {

			x = x + mx;
			y = y + my;

			int edge = size - 1;

			if (x > edge)
				x = 0;
			else if (x < 0)
				x = edge;

			if (y > edge)
				y = 0;
			else if (y < 0)
				y = edge;

			indx = y * size + x;

		}

		void Paint (int col) {

			canv [indx] = col;

		}

		void Clear () {

			fill_n(canv, area, 0);

		}

		void Render () {

			glBegin2D ();

			//-------------------//

			int offx = 0;
			int offy = 0;

			for (int i = 0; i < area; i ++) {

				if (offx >= size) {

					offx = 0;
					offy ++;

				}

				if (canv[i]) {

					int x1 = offx * px;
					int x2 = offy * px;
	
					glBoxFilled (x1, x2, x1 + px, x2 + px, canv[i]);

				}

				offx ++;

			}

			int p1 = x * px;

			int p2 = y * px;

			glBox (p1, p2, p1 + px, p2 + px, cursor);

			// border

			int border = px * size;

			glBox (1, 0, border, border, cursor);

			//-------------------//

			glEnd2D ();
			glFlush (0);

		}

		void Iterate () {

			// update //

			scanKeys ();
			int keyd = keysDown ();


			int mx = 0;
			int my = 0;

			if (keyd) {
	
				if (keyd & KEY_UP)
					my = -1;
				else if (keyd & KEY_DOWN)
					my = 1;
				else if (keyd & KEY_LEFT)
					mx = -1;
				else if (keyd & KEY_RIGHT)
					mx = 1;
	
				if (keyd & KEY_A)
					Paint (RGB15 (R, G, B));
				else if (keyd & KEY_B)
					Paint (0);
				else if (keyd & KEY_X)
					Clear ();

			}

			MovePos (mx, my);

			if (keyd)
				Render ();

		}

};

int main () {

	irqSet(IRQ_VBLANK, Vblank);

	// video stuff

	videoSetMode (MODE_5_3D);
	glScreen2D ();

	consoleDemoInit ();

	// game prep

	Painter p;

	while (1) {

		swiWaitForVBlank();

		// render //

		p.Iterate ();

		consoleClear ();

		cout << "\x1b[1;1Harrow keys to move";
		cout << "\x1b[2;1Ha to paint";
		cout << "\x1b[3;1Hb to erase";
		cout << "\x1b[4;1Hx to clear";

		cout << "\x1b[6;2HR: " << p.R;
		cout << "\x1b[7;2HG: " << p.G;
		cout << "\x1b[8;2HB: " << p.B;

	}

	return 0;

}