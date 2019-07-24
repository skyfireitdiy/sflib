
#define SF_DEBUG

#include "sf_template.hpp"

int main()
{
    std::string content = R"(
<html>
    {$
        d = data["d"];
        for k,v in d{
            print(k, v);
        }
    $}
</html>
)";
    auto ret = skyfire::sf_render(content, skyfire::sf_json() ,{"{$", "$}"});
    std::cout<<ret<<std::endl;
}