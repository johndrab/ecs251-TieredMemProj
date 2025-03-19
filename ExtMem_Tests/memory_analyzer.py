import subprocess
import re
import argparse

def run_perf(target_program, output_file="perf_output.txt"):
    #Runs perf on the target program and saves output
    perf_command = f"perf record -e cache-misses,cache-references,major-faults,minor-faults {target_program}"
    subprocess.run(perf_command, shell=True)

    report_command = f"perf report --stdio > {output_file}"
    subprocess.run(report_command, shell=True)
    print("Perf data collected and saved.")


def parse_perf_output(file="perf_output.txt"):
    #parse perf output for count values
    data = {}

    with open(file, "r") as f:
        content = f.read()

    #I have to use regex here as perf sometimes returns different titles for each event with different programs (i.e 'cache-misses:u' has extra :u)
    event_pattern = r"Samples: .*? of event '([^']+)'\n# Event count \(approx.\): (\d+)"

    #Get all matches
    matches = re.findall(event_pattern, content)

    #print(matches)
    #Store matches in the data dictionary
    for event, count in matches:
        event_name = event.split(':')[0]
        data[event_name] = int(count)

    return data



def analyze_memory_patterns(data):
    """Analyzes memory access patterns and recommends a memory policy using a holistic approach."""
    
    #Default to 0 if key does not exist (sometimes if there are no page faults aka magior faults recoreded it will not be in the report so have to set it to zero if not found to prevent error)
    cache_misses = data.get('cache-misses', 0)
    cache_references = data.get('cache-references', 0)
    major_faults = data.get('major-faults', 0)
    minor_faults = data.get('minor-faults', 0)

    print("Raw Data Values:")
    print(f"Cache Misses: {cache_misses}")
    print(f"Cache References: {cache_references}")
    print(f"Major Faults: {major_faults}")
    print(f"Minor Faults: {minor_faults}")
    print()

    #Normalize data and calculate scores
    cache_miss_ratio = cache_misses / max(1, cache_references)
    major_fault_ratio = major_faults / max(1, cache_references)
    
    #Scores for each aspect (scale 0 to 1, higher is worse)
    cache_miss_score = min(cache_miss_ratio, 1)  #Cap the score at 1 (worst case)
    major_fault_score = min(major_fault_ratio, 1)
    minor_fault_score = min(minor_faults / 10000, 1)
    
    print("Calculated Scores:")
    print(f"Cache Miss Ratio: {cache_miss_ratio:.2f}")
    print(f"Major Fault Ratio: {major_fault_ratio:.2f}")
    print(f"Minor Faults: {minor_faults}")
    print(f"Cache Miss Score: {cache_miss_score:.2f}")
    print(f"Major Fault Score: {major_fault_score:.2f}")
    print(f"Minor Fault Score: {minor_fault_score:.2f}")
    print()

    #Combining scores to make it easier to provide final recomendation as this will evaluate all of the different traits, currently have weights hardcoded 
    total_score = (cache_miss_score * 0.4) + (major_fault_score * 0.4) + (minor_fault_score * 0.2)

    print(f"Combined Score: {total_score:.2f}")
    print()

    if total_score > 0.8:
        recommendation = "Prefetching Migration"
    elif total_score > 0.6:
        recommendation = "LRU Migration"
    elif total_score > 0.4:
        recommendation = "Threshold-Based Migration"
    else:
        recommendation = "No Migration"
    return recommendation


def main():
    parser = argparse.ArgumentParser(description="Run perf on a program and analyze memory patterns.")
    parser.add_argument("program", help="The program to analyze (provide the full path if necessary).")

    args = parser.parse_args()
    target_program = args.program

    print(f"Running memory analysis on: {target_program}")
    run_perf(target_program)
    perf_data = parse_perf_output()
    recommended_policy = analyze_memory_patterns(perf_data)

    print(f"\nRecommended Memory Policy: {recommended_policy}")

if __name__ == "__main__":
    main()

