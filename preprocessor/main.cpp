#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using filesystem::path;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

bool PreprocessFile(ifstream& input, ofstream& output, const path& current_file, const vector<path>& include_directories) {
    string line;
    int line_number = 1;
    static regex inc_local(R"/(\s*#\s*include\s*"([^"]*)"\s*)/");
    static regex inc_global(R"/(\s*#\s*include\s*<([^>]*)>\s*)/");

    while (getline(input, line)) {
        smatch m;
        if (regex_match(line, m, inc_local)) {
            path include_path = current_file.parent_path() / m[1].str();
            ifstream include_input(include_path);
            if (!include_input.is_open()) {
                for (const auto& dir : include_directories) {
                    include_path = dir / m[1].str();
                    include_input.open(include_path);
                    if (include_input.is_open()) {
                        break;
                    }
                }
                if (!include_input.is_open()) {
                    cout << "unknown include file " << m[1].str() <<
                        " at file " << current_file.string() << " at line " << line_number << endl;
                    return false;
                }
            }
            if (!PreprocessFile(include_input, output, include_path, include_directories)) {
                return false;
            }
        }
        else if (regex_match(line, m, inc_global)) {
            bool found = false;
            for (const auto& dir : include_directories) {
                path include_path = dir / m[1].str();
                ifstream include_input(include_path);
                if (include_input.is_open()) {
                    if (!PreprocessFile(include_input, output, include_path, include_directories)) {
                        return false;
                    }
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "unknown include file " << m[1].str() <<
                    " at file " << current_file.string() << " at line " << line_number << endl;
                return false;
            }
        }
        else {
            output << line << endl;
        }
        ++line_number;
    }
    return true;
};

bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories) {
    ifstream input(in_file);
    if (!input.is_open()) {
        return false;
    }

    ofstream output(out_file);
    if (!output.is_open()) {
        return false;
    }

    if (in_file.empty()) {
        return false;
    }

    return PreprocessFile(input, output, in_file, include_directories);
};

string GetFileContents(string file) {
    ifstream stream(file);

    // конструируем string по двум итераторам
    return { (istreambuf_iterator<char>(stream)), istreambuf_iterator<char>() };
}

void Test() {
    error_code err;
    filesystem::remove_all("sources"_p, err);
    filesystem::create_directories("sources"_p / "include2"_p / "lib"_p, err);
    filesystem::create_directories("sources"_p / "include1"_p, err);
    filesystem::create_directories("sources"_p / "dir1"_p / "subdir"_p, err);

    {
        ofstream file("sources/a.cpp");
        file << "// this comment before include\n"
            "#include \"dir1/b.h\"\n"
            "// text between b.h and c.h\n"
            "#include \"dir1/d.h\"\n"
            "\n"
            "int SayHello() {\n"
            "    cout << \"hello, world!\" << endl;\n"
            "#   include<dummy.txt>\n"
            "}\n"s;
    }
    {
        ofstream file("sources/dir1/b.h");
        file << "// text from b.h before include\n"
            "#include \"subdir/c.h\"\n"
            "// text from b.h after include"s;
    }
    {
        ofstream file("sources/dir1/subdir/c.h");
        file << "// text from c.h before include\n"
            "#include <std1.h>\n"
            "// text from c.h after include\n"s;
    }
    {
        ofstream file("sources/dir1/d.h");
        file << "// text from d.h before include\n"
            "#include \"lib/std2.h\"\n"
            "// text from d.h after include\n"s;
    }
    {
        ofstream file("sources/include1/std1.h");
        file << "// std1\n"s;
    }
    {
        ofstream file("sources/include2/lib/std2.h");
        file << "// std2\n"s;
    }

    assert((!Preprocess("sources"_p / "a.cpp"_p, "sources"_p / "a.in"_p,
        { "sources"_p / "include1"_p,"sources"_p / "include2"_p })));

    ostringstream test_out;
    test_out << "// this comment before include\n"
        "// text from b.h before include\n"
        "// text from c.h before include\n"
        "// std1\n"
        "// text from c.h after include\n"
        "// text from b.h after include\n"
        "// text between b.h and c.h\n"
        "// text from d.h before include\n"
        "// std2\n"
        "// text from d.h after include\n"
        "\n"
        "int SayHello() {\n"
        "    cout << \"hello, world!\" << endl;\n"s;

    assert(GetFileContents("sources/a.in"s) == test_out.str());
}

int main() {
    Test();
}