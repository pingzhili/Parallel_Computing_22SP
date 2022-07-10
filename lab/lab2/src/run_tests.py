import subprocess
import re

def run_test(num_threads, delta):
    output_str = subprocess.run(['./delta_stepping_parallel', str(num_threads), str(delta)], capture_output=True, text=True).stdout
    print(output_str)
    return output_str


if __name__ == '__main__':
    times = []
    for i in range(2, 17):
        opstr = run_test(i, 6)
        time_str = re.split(r'time taken: ', opstr)[1]
        times.append(float(time_str))
    print(times)

