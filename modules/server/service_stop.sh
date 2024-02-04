#!/bin/bash

# 检查目标进程是否已经停止
if ! pgrep -x "face_id_server" > /dev/null; then
  echo "目标进程已经停止"
  exit 0
fi

# 发送 SIGINT 信号给目标进程
kill -s TERM $MAINPID

# 等待目标进程退出
wait $MAINPID