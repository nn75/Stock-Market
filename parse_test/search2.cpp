#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>


using namespace std;

int main(int argc, char **argv)
{
    xmlDocPtr pdoc = NULL;
    xmlNodePtr proot = NULL, pcur = NULL;
    /*****************打开xml文档********************/
    xmlKeepBlanksDefault(0);                                    //必须加上，防止程序把元素前后的空白文本符号当作一个node
    //pdoc = xmlReadFile("test.xml", "UTF-8", XML_PARSE_RECOVER); //libxml只能解析UTF-8格式数据
    string xmlString = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?><bookstore><book><title lang=\"eng\">Harry Potter</title><price>29.99</price></book><book><title lang=\"eng\">Learning XML</title><price>39.95</price></book></bookstore>";
    pdoc = xmlParseMemory((const char *)xmlString.c_str(), xmlString.size());

    if (pdoc == NULL)
    {
        printf("error:can't open file!\n");
        exit(1);
    }

    /*****************获取xml文档对象的根节对象********************/
    proot = xmlDocGetRootElement(pdoc);

    if (proot == NULL)
    {
        printf("error: file is empty!\n");
        exit(1);
    }

    /*****************查找书店中所有书籍的名称********************/
    pcur = proot->xmlChildrenNode;

    while (pcur != NULL)
    {
        //如同标准C中的char类型一样，xmlChar也有动态内存分配，字符串操作等 相关函数。例如xmlMalloc是动态分配内存的函数；xmlFree是配套的释放内存函数；xmlStrcmp是字符串比较函数等。
        //对于char* ch="book", xmlChar* xch=BAD_CAST(ch)或者xmlChar* xch=(const xmlChar *)(ch)
        //对于xmlChar* xch=BAD_CAST("book")，char* ch=(char *)(xch)
        if (!xmlStrcmp(pcur->name, BAD_CAST("book")))
        {
            xmlNodePtr nptr = pcur->xmlChildrenNode;
            while (pcur != NULL)
            {
                if (!xmlStrcmp(nptr->name, BAD_CAST("title")))
                {
                    printf("title: %s\n", ((char *)XML_GET_CONTENT(nptr->xmlChildrenNode)));
                    break;
                }
            }
        }
        pcur = pcur->next;
    }

    /*****************释放资源********************/
    xmlFreeDoc(pdoc);
    xmlCleanupParser();
    xmlMemoryDump();

    std::cout <<"Happy success!" << std::endl;

    return 0;
}


// using namespace std;

// int main(int argc, char *argv[]){
//    string xmlString = "<?xml version="1.0" encoding="UTF-8"?><create><account id="1" balance="10000"/></create>";

//     xmlDocPtr pdoc;
//     pdoc = xmlParseMemory((const char *)xmlString.c_str(), xmlString.size());
//     xmlNodePtr cur = xmlDocGetRootElement(doc); 
//     cout << *curr << endl;

//     return EXIT_SUCCESS;
// }


