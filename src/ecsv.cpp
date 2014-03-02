
#include <stdio.h>
#include "ecsv.hpp"

int ECsv::_open_fp()
{
	_fp = fopen(_file.c_str(), "rb");
	return (_fp) ? 0 : -1;
}

void ECsv::_close_fp()
{
	if (_fp) {
		fclose(_fp);
		_fp = NULL;
	}
}

int ECsv::open(const char *file)
{
	if (file && *file) {
		_file = file;
	}
	return _open_fp();
}

void ECsv::close()
{
	_close_fp();
}

int ECsv::getline()
{
	if (!_fp) {
		return -1;
	}

	int c;
	int quoted = 0;
	string fld = "";

	// clear field container...
	_field.clear();

	while ((c = fgetc(_fp)) != EOF) {
		// skip spaces if unquoted...
		if (isspace(c) && ! quoted) {
			if (c == '\n') {
                if (fld.size() > 0) {
                    _field.push_back(fld);
                }
				break;
			}
			continue;
		}

		// check for quotes...
		char lc = (fld.size() > 0) ? fld[fld.size()-1] : 0;
		if (c == '\"' && lc != '\\') {
			quoted = (! quoted) ? 1 : 0;
			continue;
		}

		// check for separator...
		if (c == (int)_sep && ! quoted) {
			_field.push_back(fld);
			fld = "";
			continue;
		}

		// append char to field...
		fld += (char) c;
	}

	// append remaining field to container...
	if (fld.size() > 0) {
		_field.push_back(fld);
	}
	
	return (_field.size() > 0) ? 0 : -1;
}

const char* ECsv::getfield(int n) const
{
	if (n < 0 || n >= (int)_field.size())
		return "";
	else
		return _field[n].c_str();
}

#ifdef __ECSV_MAIN__
int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("Usage: %s <file>\n", argv[0]);
		return 0;
	}
	
	ECsv *csv = new ECsv();

	if (0 != csv->open(argv[1])) {
		printf("Unable to open csv file...(%s)\n", csv->getfile());
		return 0;
	}

	while (0 == csv->getline()) {
		for (int i = 0; i < csv->getnfield(); ++i) {
			printf("f[%d]=%s, ", i, csv->getfield(i));
		}
		putchar('\n');
	}
	csv->close();

	return 0;
}
#endif
