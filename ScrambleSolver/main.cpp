#include <iostream>
#include <math.h>
#include <string>
#include <vector>
#include <Windows.h>

#define BOARD_SIZE 9

#define EARTH_NORTH 1 // 2 landmasses
#define EARTH_SOUTH -1

#define SATURN_UP 2 // the way the front ring points
#define SATURN_DOWN -2

#define JUPITER_LIGHT 3  // top right corner light
#define JUPITER_DARK -3

#define MARS_TOP 4 // very bright 
#define MARS_BOTTOM -4

/* SIDES */
// 0 - top
// 1 - right
// 2 - bottom
// 3 - left
// ** rotations are clockwise
typedef class c_square {
public:
	c_square();
	c_square(int* sides, int idx);
	void reset();
public:
	int side[4] = { 0, 0, 0, 0 };
	int index = 0;
	int rotation = 0;
} square_t;

/* PIECE ORIENTATION */
// 0 1 2 
// 3 4 5
// 6 7 8
typedef class c_board {
public:
	c_board();
	void init();
public:
	square_t square[BOARD_SIZE]; 
} board_t;

// lets keep track of how many times we attempt a match
static int match_counter = 0;
// keep track of iterations
// the only piece that needs rotated is idx 0.
// match(...) and its overrided brother handle rotations
static int counter[BOARD_SIZE] = { 0,0,0,0,0,0,0,0,0 };
// keep track of which piece we are calculating
static int step = 0;
// one array for input board, one array for output board
static board_t board, completed;
// keep track of which original pieces are used
static std::vector<int> used;

// avoid losing track of original orientation
int get_rotated_side(square_t s1, int idx); 
// in order to match two sides, the "opposite side" needs to be calculated. essentially the same as 0 deg and 180 deg
int get_opposite_side(int idx); 

// match one piece with one piece
bool match(square_t s1, square_t& s2, int idx, bool rotate = true); 
// match one piece with two pieces
bool match(square_t& main, square_t s1, square_t s2, int c, int d); 

// properly copy all data from input square to output square
void set_square(square_t& s1, square_t& s2); 
// print output. 
void print_board(board_t b = completed); 
// clear a board (i LOVE default parameters)
void erase_board(board_t& b); 
// clear a board to allow for a reiteration
void erase_board(board_t& b, int upto); 
// the HEART FUNCTION of this solver
// automatically calculates which pieces need to be matched, organizes each method into a type,
// and calculates each piece according to type.
// TYPE -1	: 0 piece (no matching)
// TYPE 0	: 1 piece matching
// TYPE 1	: 1 piece matching (match top)
// TYPE 2	: 2 piece matching (match top and left)
void calculate(int idx);
// what is the fun in completing a statistical challenge without showing the actual statistics?
void log();

// we need to format numbers, that way they look nice
std::string convert_num(int n);

int main() {
	board.init();

	while (step >= 0)
		calculate(step);

	print_board();
	log();

	Sleep(INFINITE);
	return 0;
}

void log() {
	// counter[] is an array with GAME_SIZE-1 length
	// since each number can only iterate to GAME_SIZE-1
	// before tripping the previous, this array turns
	// into a base-(GAME_SIZE-1) number

	unsigned int num = counter[BOARD_SIZE-1];
	for (int i = BOARD_SIZE - 1; i >= BOARD_SIZE - 9; i--)
		num += counter[i] * pow(BOARD_SIZE-1, -i + BOARD_SIZE - 1);

	printf("[MATH] Number of iterations: %s\n", convert_num(num).c_str());
	printf("[MATH] Number of matches tested: %s\n", convert_num(match_counter).c_str());
}

std::string convert_num(int n) {
	std::string ans = "";
	std::string num = std::to_string(n);

	int count = 0;
	for (int i = num.size() - 1;
		i >= 0; i--) {
		count++;
		ans.push_back(num[i]);

		if (count == 3) {
			ans.push_back(',');
			count = 0;
		}
	}

	reverse(ans.begin(), ans.end());

	if (ans.size() % 4 == 0) {

		// Remove ','
		ans.erase(ans.begin());
	}
	return ans;
}

void calculate(int idx) { 
	int piece = 0;
	int type = 0;
	switch (idx) {
	case 0:
		type = -1;
		break;
	case 1:
	case 2:
		type = 0;
		piece = idx - 1;
		break;
	case 3:
	case 6:
		type = 1;
		piece = idx - 3;
		break;
	case 4:
	case 5:
	case 7:
	case 8:
		type = 2;
		piece = idx - 3;
		break;
	}

	if (type == -1) {
		used.clear();
		erase_board(completed);

		board.square[counter[idx] / 4].rotation = (counter[idx] % 4);
		set_square(completed.square[idx], board.square[counter[idx] / 4]);

		counter[idx]++;
		used.push_back(counter[idx] / 4);
		step++;
	}
	else if (type == 0) {
		for (int i = counter[idx]; i <= BOARD_SIZE; i++) {
			if (i >= BOARD_SIZE) {
				used.pop_back();
				erase_board(completed, idx - 1);

				step--;
				counter[idx] = 0;
				break;
			}
			else if ((std::find(used.begin(), used.end(), i) == used.end()) &&
				match(completed.square[idx - 1], board.square[i], 1)) {
				counter[idx]++;
				used.push_back(i);

				set_square(completed.square[idx], board.square[i]);
				step++;
				break;
			}
			else
				counter[idx]++;
		}
	}
	else if (type == 1) {
		for (int i = counter[idx]; i <= BOARD_SIZE; i++) {
			if (i >= BOARD_SIZE) {
				used.pop_back();
				erase_board(completed, idx-1);

				step--;
				counter[idx] = 0;
				break;
			}
			else if ((std::find(used.begin(), used.end(), i) == used.end()) &&
				match(completed.square[piece], board.square[i], 2)) {
				counter[idx]++;
				used.push_back(i);

				set_square(completed.square[idx], board.square[i]);
				if (idx == BOARD_SIZE - 1) step = -1;
				else step++;
				break;
			}
			else
				counter[idx]++;
		}
	}
	else if (type == 2) {
		for (int i = counter[idx]; i <= BOARD_SIZE; i++) {
			if (i >= BOARD_SIZE) {
				used.pop_back();
				erase_board(completed, idx - 1);

				step--;
				counter[idx] = 0;
				break;
			}
			else if ((std::find(used.begin(), used.end(), i) == used.end()) &&
				match(board.square[i], completed.square[piece], completed.square[piece+2], 2, 1)) {
				counter[idx]++;
				used.push_back(i);

				set_square(completed.square[idx], board.square[i]);
				if (idx == BOARD_SIZE - 1) step = -1;
				else step++;
				break;
			}
			else
				counter[idx]++;
		}
	}
}

void erase_board(board_t& b) {
	for (int i = 0; i < BOARD_SIZE; i++)
		b.square[i].reset();
}

void erase_board(board_t& b, int upto) {
	for (int i = upto; i < BOARD_SIZE; i++)
		b.square[i].reset();
}

void print_board(board_t b) {
	printf("[NUMERICAL REPRESENTATION]\n");
	for (int i = 0; i < BOARD_SIZE; i++) {
		printf("[OUTPUT] Square %i:\t%i\t|\t%i\t|\t%i\t|\t%i\n",
			i,
			get_rotated_side(b.square[i], 0),
			get_rotated_side(b.square[i], 1),
			get_rotated_side(b.square[i], 2),
			get_rotated_side(b.square[i], 3)
		);
	}

	printf("\n\n"); // clear
	printf("[PHYSICAL REPRESENTATION]\n");
	printf("[INFO] P1 (R0) - Piece 1 with 0 CW rotations\n");
	for (int d = 0; d < 3; d++) {
		for (int i = 0; i < 3; i++) {
			printf("P%i (%iR)\t\t", completed.square[3*d+i].index+1, completed.square[3*d+i].rotation);
		}
		printf("\n");
	}
	printf("\n\n");
}

void set_square(square_t& s1, square_t& s2) {
	for (int i = 0; i < 4; i++) s1.side[i] = s2.side[i];
	s1.rotation = s2.rotation;
	s1.index = s2.index;
}

bool match(square_t s1, square_t& s2, int idx, bool rotate) {
	match_counter++;
	if (rotate) {
		for (int i = 0; i < 4; i++) {
			s2.rotation = i;

			if ((get_rotated_side(s1, idx) + get_rotated_side(s2, get_opposite_side(idx))) == 0) {
				return true;
				break;
			}
			else
				continue;
		}
	}
	else {
		if ((get_rotated_side(s1, idx) + get_rotated_side(s2, get_opposite_side(idx))) == 0)
			return true;
	}
	return false;
}

bool match(square_t& main, square_t s1, square_t s2, int c, int d) {
	bool ret = false;
	if (match(s1, main, c)) {
		if (match(s2, main, d, false))
			ret = true;
		else ret = false;
	}
	if (!ret && match(s2, main, d)) {
		if (match(s1, main, c, false))
			ret = true;
		else ret = false;
	}
	return ret;
}

int get_rotated_side(square_t s1, int idx) {
	int new_idx = idx - s1.rotation;
	while (new_idx < 0)
		new_idx += 4;
	while (new_idx > 3)
		new_idx -= 4;

	return s1.side[new_idx];
}

int get_opposite_side(int idx) {
	int side = idx - 2;

	while (side < 0)
		side += 4;
	while (side > 3)
		side -= 4;

	return side;
}

c_square::c_square() {}
c_square::c_square(int* sides, int idx) {
	side[0] = sides[0];
	side[1] = sides[1];
	side[2] = sides[2];
	side[3] = sides[3];

	index = idx;
}

void c_square::reset() {
	side[0] = 0;
	side[1] = 0;
	side[2] = 0;
	side[3] = 0;

	rotation = 0;
}

c_board::c_board() {}
void c_board::init() {
	this->square[0] = c_square(new int[] { JUPITER_DARK,	EARTH_NORTH,	JUPITER_LIGHT,	MARS_BOTTOM}, 0);
	this->square[1] = c_square(new int[] { JUPITER_LIGHT,	SATURN_UP,		MARS_BOTTOM,	SATURN_UP }, 1);
	this->square[2] = c_square(new int[] { MARS_TOP,		EARTH_NORTH,	EARTH_SOUTH,	SATURN_DOWN }, 2);
	this->square[3] = c_square(new int[] { MARS_BOTTOM,		SATURN_DOWN,	JUPITER_DARK,	EARTH_SOUTH}, 3);
	this->square[4] = c_square(new int[] { MARS_TOP,		JUPITER_DARK,	EARTH_SOUTH,	SATURN_UP }, 4);
	this->square[5] = c_square(new int[] { EARTH_NORTH,		SATURN_DOWN,	MARS_BOTTOM,	JUPITER_LIGHT  }, 5);
	this->square[6] = c_square(new int[] { MARS_TOP,		SATURN_UP,		EARTH_SOUTH,	JUPITER_DARK }, 6);
	this->square[7] = c_square(new int[] { EARTH_NORTH,		JUPITER_LIGHT,	MARS_BOTTOM,	SATURN_DOWN }, 7);
	this->square[8] = c_square(new int[] { MARS_TOP,		SATURN_DOWN,	EARTH_NORTH,	JUPITER_LIGHT }, 8);
}