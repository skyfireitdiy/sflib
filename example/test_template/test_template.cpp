
#define SF_DEBUG

#include "sf_template.hpp"

int main()
{
    std::string content = R"(
<html>
    {$
        print(5)
    $}
</html>
)";
    auto ret = skyfire::sf_render(content, skyfire::sf_json() ,{"{$", "$}"});
    std::cout<<ret<<std::endl;
	getchar();
}