import pandas as pd
import matplotlib.pyplot as plt

# --- Configuration ---
# The name of your data file
data_filename = 'coordinates.txt' 
plot_filename = 'graph.png'
# --- End Configuration ---

def plot_hit_rates_from_txt():
    """
    Reads a space-separated data file (with no headers) and plots 
    the hit rates for LRU and MRU against the hot mix percentage.
    
    Expected format in 'results.txt':
    <x_value> <y_value_lru> <y_value_mru>
    e.g.:
    0 0.00 0.00
    10 0.50 0.50
    ...
    """
    try:
        # Read the space-separated data file
        # We specify no header and assign column names ourselves
        df = pd.read_csv(
            data_filename,
            delim_whitespace=True,  # Use whitespace (spaces, tabs) as separator
            header=None,            # The file has no header row
            names=['hot_mix_pct', 'lru_hit_rate', 'mru_hit_rate'] # Assign names
        )
    except FileNotFoundError:
        print(f"Error: '{data_filename}' not found.")
        print("Please make sure your data file is in the same directory.")
        return
    except Exception as e:
        print(f"Error reading file: {e}")
        return

    print("Generating plot...")

    # Create the plot
    plt.figure(figsize=(12, 7))
    
    # Plot LRU
    plt.plot(df['hot_mix_pct'], df['lru_hit_rate'], label='LRU', marker='o')
    
    # Plot MRU
    plt.plot(df['hot_mix_pct'], df['mru_hit_rate'], label='MRU', marker='o')

    # Customize the plot
    plt.title('Buffer Strategy Performance (LRU vs. MRU)', fontsize=16)
    plt.xlabel('Hot Set Access Percentage (%)', fontsize=12)
    plt.ylabel('Cache Hit Rate (%)', fontsize=12)
    plt.legend(title='Strategy')
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.xticks(df['hot_mix_pct']) # Use the actual x-values for ticks
    plt.ylim(bottom=0) # Start y-axis at 0
    
    # Save the plot to a file
    plt.savefig(plot_filename)
    
    print(f"Plot saved as '{plot_filename}'")
    
    # Show the plot
    plt.show()

if __name__ == "__main__":
    plot_hit_rates_from_txt()