#Libco
Libco is a c/c++ coroutine library that is widely used in WeChat services. It has been running on tens of thousands of machines since 2013.

Author: sunnyxu(sunnyxu@tencent.com), leiffyli(leiffyli@tencent.com), dengoswei@gmail.com(dengoswei@tencent.com), sarlmolchen(sarlmolchen@tencent.com)

By linking with libco, you can easily transform synchronous back-end service into coroutine service. The coroutine service will provide out-standing concurrency compare to multi-thread approach. With the system hook, You can easily coding in synchronous way but asynchronous executed.

You can also use co_create/co_resume/co_yield interfaces to create asynchronous back-end service. These interface will give you more control of coroutines.

By libco copy-stack mode, you can easily build a back-end service support tens of millions of tcp connection.
***
### ���
libco��΢�ź�̨���ģʹ�õ�c/c++Э�̿⣬2013�������ȶ�������΢�ź�̨������̨�����ϡ�  

libcoͨ�����еļ��������ӿ� co_create/co_resume/co_yield ����� co_poll������֧��ͬ�������첽��д�������߳̿�һ�����ɡ�ͬʱ�������ṩ��socket�庯����hook��ʹ�ú�̨�߼����񼸺������޸��߼�����Ϳ�������첽�����졣

����: sunnyxu(sunnyxu@tencent.com), leiffyli(leiffyli@tencent.com), dengoswei@gmail.com(dengoswei@tencent.com), sarlmolchen(sarlmolchen@tencent.com)

### libco������
- ��������ҵ���߼����Ѷ���̡����̷߳�������Э�̷��񣬲��������õ��ٱ�����;
- ֧��CGI��ܣ����ɹ���web����(New);
- ֧��gethostbyname��mysqlclient��ssl�ȳ��õ�����(New);
- ��ѡ�Ĺ���ջģʽ���������ɽ���ǧ������(New);
- ���Ƽ���Э�̱�̽ӿ�
 * ��pthread�ӿ���ƣ�ͨ��co_create��co_resume�ȼ������ӿڼ������Э�̵Ĵ�����ָ���
 * __thread��Э��˽�б�����Э�̼�ͨ�ŵ�Э���ź���co_signal (New);
 * ���Լ����lambdaʵ�֣����Э��ԭ�ر�д��ִ�к�̨�첽���� (New);
 * ����epoll/kqueueʵ�ֵ�С����������ܣ�����ʱ������ʵ�ֵĸ����ܶ�ʱ��;
