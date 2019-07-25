
#define SF_DEBUG

#include "sf_template.hpp"

int main()
{
    std::string content = R"(
<html>
    {$
		print(555)
        range(1,10,2,5)
    $}
</html>
)";
    auto ret = skyfire::sf_render(content, skyfire::sf_json() ,{"{$", "$}"});
    std::cout<<ret<<std::endl;
	getchar();
}