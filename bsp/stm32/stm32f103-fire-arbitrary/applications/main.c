/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-5      SummerGift   change to new framework
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_gpio.h"
/* defined the LED0 pin: PF7 */
#define LED0_PIN    GET_PIN(F, 7)

int main(void)
{
    int count = 1;
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

    while (count++)
    {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }

    return RT_EOK;
}


#include <stdio.h>
#include <string.h>

#include <rtthread.h>
#include <sys/socket.h> 
#include <netdb.h>

/* RT-Thread 官网，支持 TLS 功能 */

//#define SAL_TLS_HOST    "www.rt-thread.org"
//#define SAL_TLS_PORT    443

#define SAL_TLS_HOST    "192.168.1.1"
#define SAL_TLS_PORT    8088

#define SAL_TLS_BUFSZ   1024

static const char *send_data = "GET /download/rt-thread.txt HTTP/1.1\r\n"
    "Host: www.rt-thread.org\r\n"
    "User-Agent: rtthread/4.0.1 rtt\r\n\r\n";

void sal_tls_test(void)
{
    int ret, i;
    char *recv_data;
    struct hostent *host;
    int sock = -1, bytes_received;
    struct sockaddr_in server_addr;

    /* 通过函数入口参数url获得host地址（如果是域名，会做域名解析） */
    host = gethostbyname(SAL_TLS_HOST);

    recv_data = rt_calloc(1, SAL_TLS_BUFSZ);
    if (recv_data == RT_NULL)
    {
        rt_kprintf("No memory\n");
        return;
    }

    /* 创建一个socket，类型是SOCKET_STREAM，TCP 协议, TLS 类型 */
   
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 	//if ((sock = socket(AF_INET, SOCK_STREAM, PROTOCOL_TLS)) < 0)
    {
        rt_kprintf("Socket error\n");
        goto __exit;
    }

    /* 初始化预连接的服务端地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SAL_TLS_PORT);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        rt_kprintf("Connect fail!\n");
        goto __exit;
    }

    /* 发送数据到 socket 连接 */
    ret = send(sock, send_data, strlen(send_data), 0);
    if (ret <= 0)
    {
        rt_kprintf("send error,close the socket.\n");
        goto __exit;
    }

    /* 接收并打印响应的数据，使用加密数据传输 */
    bytes_received = recv(sock, recv_data, SAL_TLS_BUFSZ  - 1, 0);
    if (bytes_received <= 0)
    {
        rt_kprintf("received error,close the socket.\n");
        goto __exit;
    }

    rt_kprintf("recv data:\n");
    for (i = 0; i < bytes_received; i++)
    {
        rt_kprintf("%c", recv_data[i]);
    }

__exit:
    if (recv_data)
        rt_free(recv_data);

    if (sock >= 0)
        closesocket(sock);
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT(sal_tls_test, SAL TLS function test);
#endif /* FINSH_USING_MSH */