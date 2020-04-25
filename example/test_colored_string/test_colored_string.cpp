#include <core/sf_colored_string>

using namespace skyfire;
using namespace std;

int main()
{
    std::string s = "hello world";
    s = sf_colored_string(s, { sf_color_style_flashing });
    cout << s << endl;
}