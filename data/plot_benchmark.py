import pandas as pd
import matplotlib.pyplot as plt
import os

DATA_FILE = "data/benchmark_data.csv"
OUTPUT_DIR = "data/plots"

if not os.path.exists(DATA_FILE):
    print(f"Error: {DATA_FILE} not found.")
    exit(1)

if not os.path.exists(OUTPUT_DIR):
    os.makedirs(OUTPUT_DIR)

# Load data
df = pd.read_csv(DATA_FILE)

# Melt the dataframe
melted_df = df.melt(
    id_vars=["Pixel Count", "Kernel Size", "Clusters", "Threads"],
    value_vars=["Multithreaded Speedup", "Distributed Speedup", "Shared Speedup"],
    var_name="Implementation",
    value_name="Speedup",
)

# Clean up implementation names
melted_df["Implementation"] = melted_df["Implementation"].str.replace(" Speedup", "")

# Get unique cluster/thread combinations
combinations = melted_df[["Clusters", "Threads"]].drop_duplicates().values

for cluster, thread in combinations:
    subset_combo = melted_df[
        (melted_df["Clusters"] == cluster) & (melted_df["Threads"] == thread)
    ]

    if subset_combo.empty:
        continue

    # Get unique kernel sizes
    kernel_sizes = sorted(subset_combo["Kernel Size"].unique())
    num_kernels = len(kernel_sizes)

    if num_kernels == 0:
        continue

    # Create subplots based on kernel sizes
    fig, axes = plt.subplots(
        1, num_kernels, figsize=(6 * num_kernels, 5), squeeze=False
    )
    fig.suptitle(f"Speedup vs Image Size (Clusters={cluster}, Threads={thread})")

    for i, k_size in enumerate(kernel_sizes):
        ax = axes[0, i]
        subset_kernel = subset_combo[subset_combo["Kernel Size"] == k_size]

        # Plot each implementation
        for impl in subset_kernel["Implementation"].unique():
            data = subset_kernel[subset_kernel["Implementation"] == impl]
            # Sort by Pixel Count to ensure line is correct
            data = data.sort_values("Pixel Count")
            ax.plot(data["Pixel Count"], data["Speedup"], marker="o", label=impl)

        ax.set_title(f"Kernel Size: {k_size}")
        ax.set_xlabel("Pixel Count")
        ax.set_ylabel("Speedup (vs Serial)")
        # Place legend on the first plot or outside
        if i == 0:
            ax.legend()
        ax.grid(True)

    plt.tight_layout()
    output_filename = f"{OUTPUT_DIR}/speedup_C{cluster}_T{thread}.png"
    plt.savefig(output_filename)
    plt.close()
    print(f"Saved plot to {output_filename}")

print("Plotting complete.")
