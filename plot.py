import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# def get_data(algo, variable='cwnd', duration=1.5):
#     # Function for getting and manipulating data.
#     file_name = f'data/Tcp{algo}-{variable}.data'
#     data = np.empty((0, 2))  # Initialize an empty 2D array with shape (0, 2)
    
#     try:
#         with open(file_name, 'r') as file:
#             for line in file:
#                 # Convert the line to a list of floats and reshape to (1, 2)
#                 row = np.array(list(map(float, line.strip().split()))).reshape(1, -1)
#                 # Append the row to the data array
#                 data = np.append(data, row, axis=0)
        
#         # Transpose the data for consistency
#         data = data.T

#         # Filter out rows where time exceeds the specified duration
#         data = data[:, data[0] < duration]
#         if len(data[0]) == 0:
#             # Add a default row if data is empty
#             data = np.append(data, np.array([[duration, 0]]).T, axis=1)
#         else:
#             # Add the last column for the given duration
#             data = np.append(data, np.array([[duration, data[1, -1]]]).T, axis=1)

#     except FileNotFoundError:
#         print(f"File not found: {file_name}")
#         data = np.array([[0, 0], [duration, 0]]).T  # Default data if file is missing
    
#     return data

# def plot_cwnd_ack_rtt_each_algorithm(duration=1.5):
#     algos = ['NewReno']
#     segment = 2500  # 1 [segment] = 340 [byte]
#     plt.figure()

#     for algo in algos:
#         plt.subplot(311)
#         cwnd_data = get_data(algo, 'cwnd', duration)
#         ssth_data = get_data(algo, 'ssth', duration)
#         ack_data = get_data(algo, 'ack', duration)  # Replacing cong-state with ack

#         ymax = max(cwnd_data[1] / segment) * 1.1

#         # Fill plot based on acknowledgment data
#         plt.fill_between(cwnd_data[0], cwnd_data[1] / segment,
#                          facecolor='lightblue')
#         for n in range(len(ack_data[0]) - 1):
#             fill_range = cwnd_data[0] >= ack_data[0, n]
#             plt.fill_between(
#                 cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
#                 facecolor='lightgreen')

#         plt.plot(cwnd_data[0], cwnd_data[1] / segment, drawstyle='steps-post',
#                  color='b', label='cwnd')
#         plt.plot(ssth_data[0], ssth_data[1] / segment, drawstyle='steps-post',
#                  color='b', linestyle='dotted', label='ssth')
#         plt.ylim(0, ymax)
#         plt.ylabel('cwnd [segment]')
#         plt.legend()
#         plt.title(algo)

#         plt.subplot(312)
#         plt.plot(ack_data[0], ack_data[1] / segment, drawstyle='steps-post',
#                  color='r')
#         plt.ylabel('ACK [segment]')

#         plt.subplot(313)
#         rtt_data = get_data(algo, 'rtt', duration)
#         plt.plot(rtt_data[0], rtt_data[1], drawstyle='steps-post', color='g')
#         plt.ylabel('RTT[s]')

#         plt.xlabel('Time [s]')
#         plt.savefig('data/Tcp' + algo + str(int(duration)).zfill(3) +
#                     '-cwnd-ack-rtt.png')
#         plt.clf()
#     plt.show(block=False)
#     plt.close()


# def plot_cwnd_all_algorithms(duration=1.5):
#     algos = ['NewReno']
#     segment = 2500  # 1 [segment] = 340 [byte]

#     plt.figure(figsize=(15, 10))
#     for i, algo in enumerate(algos):
#         plt.subplot(3, 4, i + 1)
#         cwnd_data = get_data(algo, 'cwnd', duration)
#         ssth_data = get_data(algo, 'ssth', duration)
#         ack_data = get_data(algo, 'ack', duration)  # Replacing cong-state with ack

#         plt.fill_between(cwnd_data[0], cwnd_data[1] / segment,
#                          facecolor='lightblue')
#         for n in range(len(ack_data[0]) - 1):
#             fill_range = cwnd_data[0] >= ack_data[0, n]
#             plt.fill_between(
#                 cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
#                 facecolor='lightgreen')

#         plt.plot(cwnd_data[0], cwnd_data[1] / segment, drawstyle='steps-post')
#         plt.plot(ssth_data[0], ssth_data[1] / segment, drawstyle='steps-post',
#                  color='b', linestyle='dotted')

#         plt.ylim(0, 1200)
#         plt.title(algo)

#     plt.savefig('data/TcpAll' + str(int(duration)).zfill(3) + '-cwnd.png')
#     plt.show(block=False)
#     plt.close()
def get_data(algo, variable='cwnd', duration=4.5):
    # Function for getting and manipulating data.
    file_name = f'data-udl3/Tcp{algo}-{variable}.data'
    data = np.empty((0, 2))  # Initialize an empty 2D array with shape (0, 2)
    
    try:
        with open(file_name, 'r') as file:
            for line in file:
                # Convert the line to a list of floats and reshape to (1, 2)
                row = np.array(list(map(float, line.strip().split()))).reshape(1, -1)
                # Append the row to the data array
                data = np.append(data, row, axis=0)
        
        # Transpose the data for consistency
        data = data.T

        # Filter out rows where time exceeds the specified duration
        data = data[:, data[0] < duration]
        if len(data[0]) == 0:
            # Add a default row if data is empty
            data = np.append(data, np.array([[duration, 0]]).T, axis=1)
        else:
            # Add the last column for the given duration
            data = np.append(data, np.array([[duration, data[1, -1]]]).T, axis=1)

    except FileNotFoundError:
        print(f"File not found: {file_name}")
        data = np.array([[0, 0], [duration, 0]]).T  # Default data if file is missing
    
    return data


def plot_cwnd_ack_rtt_each_algorithm_updated(duration=4.5):
    # algos = ['NewReno', 'HighSpeed', 'Hybla', 'Westwood', 'WestwoodPlus',
    #          'Vegas', 'Scalable', 'Veno', 'Bic', 'Yeah', 'Illinois', 'Htcp']
    algos = ['HighSpeed-dl','HighSpeed-ul','Hybla-dl','Hybla-ul','Veno-dl','Veno-ul','Bic-dl','Bic-ul','Yeah-dl','Yeah-ul','Illinois-dl','Illinois-ul','Htcp-dl','Htcp-ul']
    # 'Scalable-dl','Scalable-ul',
    segment = 2500  # 1 [segment] = 340 [byte]
    plt.figure()

    for algo in algos:
        plt.subplot(311)
        cwnd_data = get_data(algo, 'cwnd', duration)
        cwnd_time = np.arange(len(cwnd_data))  # 以cwnd_data行数为时间
        ssth_data = get_data(algo, 'ssth', duration)
        state_data = get_data(algo, 'cong-state', duration)

        # Since the initial value of ssthresh is too large,
        # we have to limit the range of y-axis.
        ymax = max(cwnd_data[1] / segment) * 1.1

        # Fill colors according to congestion states:
        # 0: OPEN:     blue
        # 1: DISORDER: green
        # 3: RECOVERY: yellow
        # 4: LOSS:     red
        # Initial congestion state is OPEN (1).
        plt.fill_between(cwnd_data[0], cwnd_data[1] / segment,
                         facecolor='lightblue')
        for n in range(len(state_data[0]) - 1):
            fill_range = cwnd_data[0] >= state_data[0, n]
            num_points = fill_range.sum()
           
            if state_data[1, n] == 1:
                if num_points < 10 and num_points > 0:
                    plt.scatter(cwnd_time[0,fill_range], cwnd_data[1, fill_range] / segment,
                                color='lightgreen', label='DISORDER (points)' )
                else:
                    plt.fill_between(
                        cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                        facecolor='lightgreen')
            elif state_data[1, n] == 0:
                plt.fill_between(
                    cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                    facecolor='lightblue')
            elif state_data[1, n] == 3:
                if num_points < 10 and num_points > 0:
                    plt.scatter(cwnd_time[0,fill_range], cwnd_data[1, fill_range] / segment,
                                color='khaki', label='RECOVERY (points)')
                else:
                    plt.fill_between(
                        cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                        facecolor='khaki')
            elif state_data[1, n] == 4:
                # Debug填充区域的时间段和范围
                print("fill_range indices:", fill_range.nonzero())
                print("Time range for LOSS:", cwnd_data[0][fill_range])
                print("CWND values for LOSS:", cwnd_data[1][fill_range])

                print("fill_range:", fill_range)
                print(num_points)
                if num_points < 10 and num_points > 0:
                    plt.scatter(cwnd_time[0,fill_range], cwnd_data[1, fill_range] / segment,
                                color='lightcoral', alpha=0.5, label='LOSS (points)')
                else:
                    # print(1)
                    # plt.fill_between(
                    #     cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                    #     facecolor='lightcoral')
                    plt.scatter(
                        cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                        color='lightcoral', label='LOSS')

                # plt.scatter(cwnd_time[0,fill_range], cwnd_data[1, fill_range] / segment,
                #                 color='lightcoral', label='LOSS (points)')
            # else:  # OPEN
            #     plt.fill_between(
            #         cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
            #         facecolor='red')
        plt.plot(cwnd_data[0], cwnd_data[1] / segment, drawstyle='steps-post',
                 color='b', label='cwnd')
        plt.plot(ssth_data[0], ssth_data[1] / segment, drawstyle='steps-post',
                 color='b', linestyle='dotted', label='ssth')
        plt.ylim(0, ymax)
        plt.ylabel('cwnd [segment]')
        plt.legend()
        plt.title(algo)

        plt.subplot(312)
        ack_data = get_data(algo, 'ack', duration)
        plt.plot(ack_data[0], ack_data[1] / segment, drawstyle='steps-post',
                 color='r')
        plt.ylabel('ACK [segment]')

        plt.subplot(313)
        rtt_data = get_data(algo, 'rtt', duration)
        plt.plot(rtt_data[0], rtt_data[1], drawstyle='steps-post', color='g')
        plt.ylabel('RTT[s]')

        plt.xlabel('Time [s]')
        plt.savefig('data-udl2/Tcp' + algo + str(int(duration)).zfill(3) +
                    '-cwnd-ack-rtt.png')
        plt.clf()
    plt.show(block=False)
    plt.close()

import matplotlib.pyplot as plt

def plot_cwnd_with_states(segment=2500, duration=4.5):

    cwnd_data = pd.read_csv('data-udl/TcpNewReno-dl-cwnd.data', delim_whitespace=True, header=None, names=['time', 'cwnd'])
    cwnd_time = cwnd_data['time'].values
    cwnd_values = cwnd_data['cwnd'].values
    cong_state_data = pd.read_csv('data-udl/TcpNewReno-dl-cong-state.data', delim_whitespace=True, header=None, names=['time', 'state'])
    state_time = cong_state_data['time'].values
    state_values = cong_state_data['state'].values
    # ssth_data = pd.read_csv('data-udl/TcpNewReno-dl-ssth.data', delim_whitespace=True, header=None, names=['time', 'ssth'])
    cwnd_time = np.arange(len(cwnd_data))  # 以cwnd_data行数为时间

    # Since the initial value of ssthresh is too large,
    # we have to limit the range of y-axis.
    ymax = max(cwnd_values / segment) * 1.1

    plt.figure(figsize=(10, 6))
    plt.title('Congestion Window with States')

    # Plot the congestion window as steps
    plt.plot(cwnd_time, cwnd_values / segment, drawstyle='steps-post', label='cwnd', color='blue')

    # Iterate through congestion states and replace fill with scatter
    for i in range(len(state_time) - 1):
        state_start = state_time[i]
        state_end = state_time[i + 1]
        fill_range = (cwnd_time >= state_start) & (cwnd_time < state_end)
        num_points = fill_range.sum()

        # Debug信息
        print(f"State {state_values[i]}: Filling from {state_start} to {state_end}, {num_points} points in range.")

        if state_values[i] == 4:  # LOSS
            plt.scatter(
                cwnd_time[fill_range],  # 时间轴
                cwnd_values[fill_range] / segment,  # 拥塞窗口大小
                color='lightcoral',  # 点颜色
                label='LOSS' if i == 0 else None,  # 图例
                s=10,  # 点大小
                alpha=0.7  # 透明度
            )
        elif state_values[i] == 1:  # DISORDER
            plt.scatter(
                cwnd_time[fill_range],
                cwnd_values[fill_range] / segment,
                color='lightgreen',
                label='DISORDER' if i == 0 else None,
                s=10,
                alpha=0.7
            )
        elif state_values[i] == 3:  # RECOVERY
            plt.scatter(
                cwnd_time[fill_range],
                cwnd_values[fill_range] / segment,
                color='khaki',
                label='RECOVERY' if i == 0 else None,
                s=10,
                alpha=0.7
            )
        else:  # OPEN
            plt.scatter(
                cwnd_time[fill_range],
                cwnd_values[fill_range] / segment,
                color='lightblue',
                label='OPEN' if i == 0 else None,
                s=10,
                alpha=0.7
            )

    # Add labels and legends
    plt.xlabel('Time [s]')
    plt.ylabel('Congestion Window [segments]')
    plt.ylim(0, ymax)
    plt.legend()
    plt.grid(True)
    plt.show()


def plot_cwnd_ack_rtt_each_algorithm_ori(duration=4.5):
    # algos = ['NewReno', 'HighSpeed', 'Hybla', 'Westwood', 'WestwoodPlus',
    #          'Vegas', 'Scalable', 'Veno', 'Bic', 'Yeah', 'Illinois', 'Htcp']
    # algos = ['Veno-dl','Veno-ul']
    # algos = ['NewReno-dl','NewReno-ul','WestwoodPlus-dl','WestwoodPlus-ul','Vegas-dl','Vegas-ul','HighSpeed-dl','HighSpeed-ul','Hybla-dl','Hybla-ul','Scalable-dl','Scalable-ul','Veno-dl','Veno-ul','Bic-dl','Bic-ul','Yeah-dl','Yeah-ul','Illinois-dl','Illinois-ul','Htcp-dl','Htcp-ul','Bbr-dl','Bbr-ul','Cubic-dl','Cubic-ul']
    # algos = ['NewReno-dl','NewReno-ul']
    # algos = ['WestwoodPlus-dl','WestwoodPlus-ul']
    algos = ['Veno-dl','Veno-ul']
    segment = 2500  # 1 [segment] = 340 [byte]
    plt.figure()

    for algo in algos:
        plt.subplot(311)
        cwnd_data = get_data(algo, 'cwnd', duration)
        ssth_data = get_data(algo, 'ssth', duration)
        state_data = get_data(algo, 'cong-state', duration)

        # Since the initial value of ssthresh is too large,
        # we have to limit the range of y-axis.
        ymax = max(cwnd_data[1] / segment) * 1.1

        # Fill colors according to congestion states:
        # 0: OPEN:     blue
        # 1: DISORDER: green
        # 3: RECOVERY: yellow
        # 4: LOSS:     red
        # Initial congestion state is OPEN (1).
        plt.fill_between(cwnd_data[0], cwnd_data[1] / segment,
                         facecolor='lightblue')
        for n in range(len(state_data[0]) - 1):
            fill_range = cwnd_data[0] >= state_data[0, n]
            if state_data[1, n] == 1:
                plt.fill_between(
                    cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                    facecolor='lightgreen')
            elif state_data[1, n] == 3:
                plt.fill_between(
                    cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                    facecolor='khaki')
            elif state_data[1, n] == 4:
                plt.fill_between(
                    cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                    facecolor='lightcoral')
            elif state_data[1, n] == 0:
                plt.fill_between(
                    cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                    facecolor='lightblue')
            else:  # OPEN
                plt.fill_between(
                    cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                    facecolor='lightblue')
        plt.plot(cwnd_data[0], cwnd_data[1] / segment, drawstyle='steps-post',
                 color='b', label='cwnd')
        plt.plot(ssth_data[0], ssth_data[1] / segment, drawstyle='steps-post',
                 color='b', linestyle='dotted', label='ssth')
        plt.ylim(0, ymax)
        plt.ylabel('cwnd [segment]')
        plt.legend()
        plt.title(algo)

        plt.subplot(312)
        ack_data = get_data(algo, 'ack', duration)
        plt.plot(ack_data[0], ack_data[1] / segment, drawstyle='steps-post',
                 color='r')
        plt.ylabel('ACK [segment]')

        plt.subplot(313)
        rtt_data = get_data(algo, 'rtt', duration)
        plt.plot(rtt_data[0], rtt_data[1], drawstyle='steps-post', color='g')
        plt.ylabel('RTT[s]')

        plt.xlabel('Time [s]')
        plt.savefig('data-udl3/Tcp' + algo + str(int(duration)).zfill(3) +
                    '-cwnd-ack-rtt.png')
        plt.clf()
    plt.show(block=False)
    plt.close()


def plot_cwnd_ack_rtt_each_algorithm(duration=4.5):
    # algos = ['NewReno', 'HighSpeed', 'Hybla', 'Westwood', 'WestwoodPlus',
    #          'Vegas', 'Scalable', 'Veno', 'Bic', 'Yeah', 'Illinois', 'Htcp']
    # algos = ['NewReno-dl','NewReno-ul']
    # algos = ['WestwoodPlus-dl','WestwoodPlus-ul']
    # algos = ['Vegas-dl','Vegas-ul']
    # algos = ['HighSpeed-dl','HighSpeed-ul','Hybla-dl','Hybla-ul','Scalable-dl','Scalable-ul','Veno-dl','Veno-ul','Bic-dl','Bic-ul','Yeah-dl','Yeah-ul','Illinois-dl','Illinois-ul','Htcp-dl','Htcp-ul']
    # algos = ['Bbr-dl','Bbr-ul']
    # algos = ['Cubic-dl','Cubic-ul']
    # algos = ['NewReno-dl','NewReno-ul','WestwoodPlus-dl','WestwoodPlus-ul','Vegas-dl','Vegas-ul','HighSpeed-dl','HighSpeed-ul','Hybla-dl','Hybla-ul','Scalable-dl','Scalable-ul','Veno-dl','Veno-ul','Bic-dl','Bic-ul','Yeah-dl','Yeah-ul','Illinois-dl','Illinois-ul','Htcp-dl','Htcp-ul','Bbr-dl','Bbr-ul','Cubic-dl','Cubic-ul']
    algos = ['Veno-dl','Veno-ul']
    segment = 2500  # 1 [segment] = 340 [byte]
    plt.figure()

    for algo in algos:
        plt.subplot(311)
        cwnd_data = get_data(algo, 'cwnd', duration)
        ssth_data = get_data(algo, 'ssth', duration)
        state_data = get_data(algo, 'cong-state', duration)

        # 提取数据列
        cwnd_time = cwnd_data[0]
        cwnd_values = cwnd_data[1]
        state_time = state_data[0]
        state_values = state_data[1]

        # Since the initial value of ssthresh is too large,
        # we have to limit the range of y-axis.
        ymax = max(cwnd_data[1] / segment) * 1.1

        # Fill colors according to congestion states:
        # 0: OPEN:     blue
        # 1: DISORDER: green
        # 3: RECOVERY: yellow
        # 4: LOSS:     red
        # Initial congestion state is OPEN (1).
        plt.fill_between(cwnd_data[0], cwnd_data[1] / segment,
                         facecolor='lightblue')
        for n in range(len(state_data[0]) - 1):
            # fill_range = cwnd_data[0] >= state_data[0, n]
            fill_range = (cwnd_data[0] >= state_data[0, n]) & (cwnd_data[0] < state_data[0, n+1]) 
            # state_data[1, n]第n个状态
            # state_data[0, n]第n个状态的时间
            range_time = cwnd_time[fill_range]
            range_values = cwnd_values[fill_range] / segment

           
            if state_data[1, n] == 1:
                # judge the value of points in the range is vertical or horizontal
                # 判断水平变化还是垂直变化
                for j in range(len(range_time) - 1):
                    if range_values[j] == range_values[j + 1]:  # 水平变化，填充
                        # plt.fill_between(range_time[j:j + 2], range_values[j:j + 2], color=state_colors[state], alpha=0.7)
                        continue
                    else:  # 垂直变化，用点标记
                        plt.scatter(range_time[j], range_values[j], color='lightgreen', s=10, alpha=0.8, label=None)
                plt.fill_between(
                    cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                    facecolor='lightgreen')
            elif state_data[1, n] == 3:
                print(1)
                for j in range(len(range_time) - 1):
                    if range_values[j] == range_values[j + 1]:  # 水平变化，填充
                        # plt.fill_between(range_time[j:j + 2], range_values[j:j + 2], color=state_colors[state], alpha=0.7)
                        continue
                    else:  # 垂直变化，用点标记
                        print(2)
                        plt.scatter(range_time[j], range_values[j], color='khaki', s=10, alpha=0.8, label=None)
                plt.fill_between(
                    cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                    facecolor='khaki')
            elif state_data[1, n] == 4:
                for j in range(len(range_time) - 1):
                    if range_values[j] == range_values[j + 1]:  # 水平变化，填充
                        # plt.fill_between(range_time[j:j + 2], range_values[j:j + 2], color=state_colors[state], alpha=0.7)
                        continue
                    else:  # 垂直变化，用点标记
                        plt.scatter(range_time[j], range_values[j], color='lightcoral', s=10, alpha=0.8, label=None)
                plt.fill_between(
                    cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                    facecolor='lightcoral')
            else:  # OPEN
                plt.fill_between(
                    cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                    facecolor='lightblue')
        plt.plot(cwnd_data[0], cwnd_data[1] / segment, drawstyle='steps-post',
                 color='b', label='cwnd')
        plt.plot(ssth_data[0], ssth_data[1] / segment, drawstyle='steps-post',
                 color='b', linestyle='dotted', label='ssth')
        plt.ylim(0, ymax)
        plt.ylabel('cwnd [segment]')
        plt.legend()
        plt.title(algo)

        plt.subplot(312)
        ack_data = get_data(algo, 'ack', duration)
        plt.plot(ack_data[0], ack_data[1] / segment, drawstyle='steps-post',
                 color='r')
        plt.ylabel('ACK [segment]')

        plt.subplot(313)
        rtt_data = get_data(algo, 'rtt', duration)
        plt.plot(rtt_data[0], rtt_data[1], drawstyle='steps-post', color='g')
        plt.ylabel('RTT[s]')

        plt.xlabel('Time [s]')
        plt.savefig('data-udl2/Tcp' + algo + str(int(duration)).zfill(3) +
                    '-cwnd-ack-rtt.png')
        plt.clf()
    plt.show(block=False)
    plt.close()


def plot_cwnd_all_algorithms(duration=4.5):
    # algos = ['NewReno', 'HighSpeed', 'Hybla', 'Westwood', 'WestwoodPlus',
    #          'Vegas', 'Scalable', 'Veno', 'Bic', 'Yeah', 'Illinois', 'Htcp']
    # algos = ['WestwoodPlus','Cubic','NewReno','Veno','Htcp','Yeah','Scalable','Bic','Illinois','Bbr','Vegas','HighSpeed']
    algos = ['NewReno-dl','NewReno-ul','WestwoodPlus-dl','WestwoodPlus-ul','HighSpeed-dl','HighSpeed-ul','Hybla-dl','Hybla-ul','Veno-dl','Veno-ul','Bic-dl','Bic-ul','Yeah-dl','Yeah-ul','Htcp-dl','Htcp-ul','Bbr-dl','Bbr-ul','Cubic-dl','Cubic-ul']
    segment = 2500  # 1 [segment] = 340 [byte]

    plt.figure(figsize=(15, 10))
    # plt.figure(figsize=(20, 16))
    # plt.figure(figsize=(15, 12))
    for i, algo in enumerate(algos):
        # plt.subplot(3, 4, i + 1)
        plt.subplot(5, 4, i + 1)
        cwnd_data = get_data(algo, 'cwnd', duration)
        ssth_data = get_data(algo, 'ssth', duration)
        state_data = get_data(algo, 'cong-state', duration)

        # Fill colors according to congestion states:
        # 0: OPEN:     blue
        # 1: DISORDER: green
        # 3: RECOVERY: yellow
        # 4: LOSS:     red
        # Initial congestion state is OPEN (1).
        plt.fill_between(cwnd_data[0], cwnd_data[1] / segment,
                         facecolor='lightblue')
        for n in range(len(state_data[0]) - 1):
            fill_range = cwnd_data[0] >= state_data[0, n]
            if state_data[1, n] == 1:
                plt.fill_between(
                    cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                    facecolor='lightgreen')
            elif state_data[1, n] == 3:
                plt.fill_between(
                    cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                    facecolor='khaki')
            elif state_data[1, n] == 4:
                plt.fill_between(
                    cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                    facecolor='lightcoral')
            else:
                plt.fill_between(
                    cwnd_data[0, fill_range], cwnd_data[1, fill_range] / segment,
                    facecolor='lightblue')
        plt.plot(cwnd_data[0], cwnd_data[1] / segment, drawstyle='steps-post')
        plt.plot(ssth_data[0], ssth_data[1] / segment, drawstyle='steps-post',
                 color='b', linestyle='dotted')

        # Since the initial value of ssthresh is too big,
        # we have to limit the range of y-axis.
        # plt.ylim(0, 1200)
        plt.ylim(0, 1200)
        plt.title(algo)
        
    # plt.tight_layout()
    plt.subplots_adjust(hspace=0.5)  # Adjust vertical spacing
    plt.subplots_adjust(wspace=0.3)  # Adjust horizontal spacing
    plt.savefig('data-udl3/TcpAll' + str(int(duration)).zfill(3) + '-cwnd.png')
    plt.show(block=False)
    plt.close()

def plot_tcp_aggregated_throughput(algos, ul_aggregated, dl_aggregated):
    """
    Draws a bar chart comparing UL and DL aggregated throughputs
    for multiple TCP algorithms.

    Parameters:
    - algos: List of TCP algorithm names.
    - ul_aggregated: List of UL aggregated throughput values for each algorithm.
    - dl_aggregated: List of DL aggregated throughput values for each algorithm.
    """
    x = np.arange(len(algos))  # Algorithm positions
    width = 0.35  # Bar width

    fig, ax = plt.subplots(figsize=(12, 6))

    # Plot UL and DL aggregated throughput
    ul_bars = ax.bar(x - width / 2, ul_aggregated, width, label='UL Aggregated Throughput', color='skyblue')
    dl_bars = ax.bar(x + width / 2, dl_aggregated, width, label='DL Aggregated Throughput', color='lightcoral')

    # Add labels on top of each bar
    for bar in ul_bars:
        ax.text(bar.get_x() + bar.get_width() / 2, bar.get_height() + 0.5, 
                f'{bar.get_height():.2f}', ha='center', va='bottom', fontsize=8)
    for bar in dl_bars:
        ax.text(bar.get_x() + bar.get_width() / 2, bar.get_height() + 0.5, 
                f'{bar.get_height():.2f}', ha='center', va='bottom', fontsize=8)

    # Chart details
    ax.set_title('Aggregated Throughput Comparison (UL vs DL)')
    ax.set_ylabel('Throughput (Mbps)')
    ax.set_xticks(x)
    ax.set_xticklabels(algos, rotation=45)
    ax.set_xlabel('TCP Algorithms')
    ax.legend()
    ax.grid(axis='y', linestyle='--', alpha=0.7)
    ax.set_axisbelow(True)  # Ensures grid lines are drawn behind bars

    plt.tight_layout()
    plt.savefig('data-udl3/TcpAggregatedThroughput.png')
    plt.show()

if __name__ == '__main__':
    print("----- Plotting cwnd of all algorithms -----")
    # algo = 'NewReno'
    # duration = 20.
    # cwnd_data = get_data(algo, 'cwnd', duration)
    # ssth_data = get_data(algo, 'ssth', duration)
    # state_data = get_data(algo, 'cong-state', duration)
    # print("state_data:", state_data)

    # print("cwnd_data shape:", cwnd_data.shape)
    # print("ssth_data shape:", ssth_data.shape)
    # print("state_data shape:", state_data.shape)

    # plot_cwnd_all_algorithms()
    print("----- Plotting cwnd, ACK, and RTT of each algorithm -----")
    # plot_cwnd_all_algorithms()
    # plot_cwnd_ack_rtt_each_algorithm()
    # plot_cwnd_ack_rtt_each_algorithm_ori()
    # plot_cwnd_ack_rtt_each_algorithm_updated()
    # plot_cwnd_with_states()

    algos = ['NewReno','WestwoodPlus','HighSpeed','Hybla','Veno','Bic','Yeah','Htcp','Bbr','Cubic']
    ul_aggregated = [56.4351, 23.8699, 21.8111, 56.4351, 56.4351, 34.0967, 68.8272, 56.4351, 73.158, 55.3412]
    dl_aggregated = [11.4048, 23.8138, 26.6804, 13.0934, 11.4048, 30.3156, 63.7615, 11.2365, 49.2096, 72.0417]
    plot_tcp_aggregated_throughput(algos, ul_aggregated, dl_aggregated)