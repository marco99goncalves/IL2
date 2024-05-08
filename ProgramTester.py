import subprocess
from colorama import Fore, Style
import time
from concurrent.futures import ProcessPoolExecutor, as_completed

def run_program(algorithm, probability, n_times):
    command = ["./mutator", str(probability), str(algorithm)]
    crash_count = 0
    errors = []
    for i in range(n_times):
        try:
            result = subprocess.run(command, check=True, text=True, capture_output=True)
            # print(Fore.GREEN + f"Algorithm {algorithm}, Probability {probability}, Run {i+1}: Success" + Style.RESET_ALL)
        except subprocess.CalledProcessError as e:
            crash_count += 1
            errors.append((i+1, e.stderr.strip()))
            print(Fore.RED + f"Algorithm {algorithm}, Probability {probability}, Run {i+1}: Crashed. Error: {e.stderr.strip()}" + Style.RESET_ALL)

    return crash_count, errors, algorithm, probability

if __name__ == "__main__":
    probabilities = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
    algorithms = [0, 1, 2]
    n_times = 10000
    totalSucess = 0
    totalCrash = 0
    start_time = time.time()
    times = []

    with ProcessPoolExecutor() as executor:
        futures = []
        for algorithm in algorithms:
            for probability in probabilities:
                futures.append(executor.submit(run_program, algorithm, probability, n_times))

        for future in as_completed(futures):
            crash_count, errors, algorithm, probability = future.result()
            totalSucess += n_times - crash_count
            totalCrash += crash_count
            if errors:
                print("Error details for Algorithm", algorithm, "and Probability", probability, ":")
                for run_number, error in errors:
                    print(Fore.RED + f"Run {run_number}: {error}" + Style.RESET_ALL)

    end_time = time.time()
    total_time = end_time - start_time

    print("=====================================")
    print("Summary")
    print(f"Total successful runs: {Fore.GREEN}{totalSucess}" + Style.RESET_ALL)
    print(f"Total crashes: {Fore.RED}{totalCrash}" + Style.RESET_ALL)
    print(f"Total time taken: {round(total_time, 2)} seconds")
    print("=====================================")
