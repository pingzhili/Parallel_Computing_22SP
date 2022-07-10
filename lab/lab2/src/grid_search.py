from os import times_result
import subprocess
import re

def run_test(num_threads, delta, threshold):
    output_str = subprocess.run(['./delta_stepping_parallel', str(num_threads), str(delta), str(threshold)], capture_output=True, text=True).stdout
    print(output_str)
    return output_str

if __name__ == '__main__':
    delta = [3, 6, 10, 30, 60, 100, 300, 600, 1000]
    threshold = [32, 64, 128, 256, 1024, 2048, 3072]
    times = {}
    for d in delta:
        for t in threshold:
            opstr = run_test(8, d, t)
            time_str = re.split(r'time taken: ', opstr)[1]
            times[(d, t)] = float(time_str)