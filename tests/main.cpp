#include <fstream>
#include <random>
#include <iostream>

int main(int argc, char **argv) {

	const char *idstr = "TRAINING";
	unsigned long num_pts, num_dim;
	double id;
	float *pts;
	unsigned int i;

	num_dim = 2;

	std::ifstream in("/dev/urandom", std::ios::binary);
	in.read((char *)&id, sizeof(id));
	in.read((char *)&num_pts, sizeof(num_pts));
	num_pts &= 0xF;
	pts = new float [num_pts * num_dim];
	for(i = 0; i < num_pts * num_dim; i++) {
		in.read((char *)&pts[i], sizeof(pts[i]));
	}
	in.close();

	std::ofstream out(argv[1], std::ios::binary);
	out.write(idstr, 8);
	out.write((char *)&id, sizeof(id));
	out.write((char *)&num_pts, sizeof(num_pts));
	out.write((char *)&num_dim, sizeof(num_dim));
	for(i = 0; i < num_pts * num_dim; i++) {
		out.write((char *)&pts[i], sizeof(pts[i]));
	}
	out.close();

	return 0;
}
