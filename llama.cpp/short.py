
import psutil
import pandas as pd
import matplotlib.pyplot as plt
from statsmodels.tsa.arima.model import ARIMA
from datetime import datetime, timedelta
import threading
import time
import multiprocessing
from multiprocessing import Manager
import pickle
import pandas as pd
import os
from selective_context import SelectiveContext
sc = SelectiveContext(model_type='gpt2', lang='en')
def suoju(a):
    
    with open('/mnt/nvme2n1/cyl/cyl/cLLama-CPU/llama.cpp/application.log', 'r') as f: 
        file_data = f.read() 
    text = file_data 
    if a==0:
        text='system - <|im_start|>system Hello,world!<|im_end|>'
    context, reduced_content = sc(text)
    print(context)
    return context
    # print(reduced_content)




# 监控系统资源使用情况的函数，运行在单独的线程中
def monitor_system_usage(interval=1,file_path='system_use.csv'):
    count=0
    while True:
        try:
            cpu_percent = psutil.cpu_percent(interval=1)
            timestamp = datetime.now()
            data_point ={'timestamp': timestamp, 'cpu_usage': cpu_percent}
            data_frame = pd.DataFrame([data_point],index=[data_point['timestamp']])
            count+=1
            if not os.path.exists(file_path):
                data_frame.to_csv(file_path, mode='w', index=False, header=True)
            else:
                data_frame.to_csv(file_path, mode='a', index=False, header=False)
            time.sleep(interval)
        except KeyboardInterrupt:
            print("Caught KeyboardInterrupt, terminating processes.")
            return 0



def build_predict_model(file_path='system_use.csv', order=(5, 1, 0)):
    start_time = time.time()  # 记录开始时间
    # 从CSV文件中读取数据
    data = pd.read_csv(file_path)
    if data.empty:
        print("No data collected yet.")
        return None, None
    data['timestamp'] = pd.to_datetime(data['timestamp'])
    data.set_index('timestamp', inplace=True)
    data.index = pd.DatetimeIndex(data.index).to_period('s')  # 将索引转换为周期
    
    # 建立ARIMA模型
    
    model = ARIMA(data['cpu_usage'], order=order)
    model_fit = model.fit()
    # 预测未来5个时间点的CPU使用率
    forecast = model_fit.forecast(steps=5)
    end_time = time.time()  # 记录结束时间
    print(f"执行时间：{end_time - start_time}秒")
    # 生成预测结果的索引
    last_column = forecast.tolist()
    # print(last_column)
    return  last_column

# process = multiprocessing.Process(target=monitor_system_usage, args=(1,'system_use.csv'))
# process.start()
suoju(0)
build_predict_model()