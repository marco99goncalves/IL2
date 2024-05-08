import subprocess
from colorama import Fore, Style
import time

def run_program(command, n_times):
    crash_count = 0
    errors = []

    for i in range(n_times):
        try:
            # Run the command and wait for it to complete
            result = subprocess.run(command, check=True, text=True, capture_output=True)
            print(Fore.GREEN + f"Run {i+1}: Success" + Style.RESET_ALL)
        except subprocess.CalledProcessError as e:
            # Increment the crash counter and log the error
            crash_count += 1
            errors.append((i+1, e.stderr.strip()))
            print(Fore.RED + f"Run {i+1}: Crashed. Error: {e.stderr.strip()}" + Style.RESET_ALL)

    return crash_count, errors

if __name__ == "__main__":
    probabilities = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
    algorithms = [0, 1, 2]
    n_times = 100

    totalSucess = 0
    totalCrash = 0
    
    start_time = time.time()

    for algorithm in algorithms:
        print("=====================================")
        print(f"Running with algorithm {algorithm}")
        for probability in probabilities:
            print("=====================================")
            print(f"Running with probability {probability}")
            command = ["./mutator", str(probability), str(algorithm)]

            crash_count, errors = run_program(command, n_times)
            totalSucess += n_times - crash_count
            totalCrash += crash_count
            print(f"Total runs: {n_times}")
            print(f"Total crashes: {crash_count}")
            if errors:
                print("Error details:")
                for run_number, error in errors:
                    print(Fore.RED + f"Run {run_number}: {error}" + Style.RESET_ALL)
            print("=====================================\n")
        print("=====================================")

    

    end_time = time.time()
    total_time = end_time - start_time

    print("=====================================")
    print("Summary")
    print(f"Total successful runs: {Fore.GREEN}{totalSucess}" + Style.RESET_ALL)
    print(f"Total crashes: {Fore.RED}{totalCrash}" + Style.RESET_ALL)
    print(f"Total time taken: {round(total_time, 2)} seconds")
    print("=====================================")