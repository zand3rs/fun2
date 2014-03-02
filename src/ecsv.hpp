
#ifndef ECSV_HPP
#define ECSV_HPP

#include <string>
#include <vector>

using namespace std;

class ECsv {

	public:
		ECsv(const char *file = "", char sep = ',') :
			_file(file), _sep(sep), _fp(NULL) {}

		~ECsv() { _close_fp(); }

		int open(const char *file = "");
		void close();
		int getline();
		const char* getfield(int n) const;
		int getnfield() const { return _field.size(); }
		const char* getfile() const { return _file.c_str(); }

	private:
		vector<string> _field;
		string _file;
		char _sep;
		FILE* _fp;

		int _open_fp();
		void _close_fp();
};

#endif //!ECSV_H
