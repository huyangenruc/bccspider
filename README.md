bccspider
=========
区域网络爬虫，主要解决结构化采集，例如电商数据，招聘网站数据，论坛等。主要特点：

1、基于qtwebkit浏览器内核开发，内置js引擎，解决各种需要js触发的事件，如需要点击出发js才生成的下一页等问题

2、采集效率高。通过xpath划定采集区域,限定采集范围，对比bfs明显提高命中率，除了点击下一页操作所有的url都是有效url

3、数据的提取，获取新url都通过js触发执行而不是传统的正则匹配

4、采集的json数据实时post到elasticsearch中，集成ik中文分词，实现准实时检索

5、url通过BloomFilter过滤,通过设置预估url爬取规模和false positive 概率控制内存的使用

6、由于QWebview只能在主线程中实例化，只能采用多进程的方式实现并行采集，进程间通过共享内存或者localsocket通信

7、配置简单，简单的xml流程图就可以实现某些站点的全网采集。可参考lagou.xml配置图。

