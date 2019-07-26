
#define SF_DEBUG

#include "sf_template.hpp"

int main()
{
    std::string content = R"(
<html>
    {$
        {
            prefix = "info:"
            for k,v in data["d"] {
                t = k + v
                v = u / 8
                print(a,2)
                print(a/8,pow(2,3),c+8)
                f( p + 6 , 7+p)
            }
        }
    $}
</html>
)";
    auto ret = skyfire::sf_render(content, skyfire::sf_json() ,{"{$", "$}"});
    std::cout<<ret<<std::endl;
	getchar();
}