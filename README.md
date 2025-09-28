# AI for Link Adaptation and Energy Prediction in Realistic 5G Networks

**Author:** Alka Valiparambil Narendran  
**Program:** Erasmus Mundus Joint Masters Degree - Green Networking and Cloud Computing
**University:** Lulea technological University | Universite de lorraine | Leeds Beckett Uniersity
**Supervisor:** Prof. Dr. Engeny Osipov (Luleå University of Technology, Sweden)

🛠️ Technologies & Tools
![alt text](https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white)
![alt text](https://img.shields.io/badge/PyTorch-EE4C2C?style=for-the-badge&logo=pytorch&logoColor=white)
![alt text](https://img.shields.io/badge/Jupyter-F37626?style=for-the-badge&logo=jupyter&logoColor=white)
![alt text](https://img.shields.io/badge/ns--3-007ACC?style=for-the-badge)
![alt text](https://img.shields.io/badge/5G--LENA-8A2BE2?style=for-the-badge)

---

## 🎯 Project Goal

This repository contains the complete work for the Master's thesis, "AI for Link Adaptation and Energy Prediction in Realistic 5G Networks." The research is split into two primary investigations:

1.  **Predictive Link Adaptation:** Developing an AI model to proactively predict the optimal Modulation and Coding Scheme (MCS) using simulated time-series data from the 5G-LENA (ns-3) framework.
2.  **Energy Consumption Forecasting:** Creating a high-accuracy model to forecast the energy usage of 5G base stations using a real-world operational dataset.

The ultimate goal is to provide foundational, data-driven components for a future system that can co-optimize network performance and energy efficiency. This repository is structured to help future researchers understand the methodology, reproduce the results, and build upon this work.

### **📈 Key Findings**
*   **Link Adaptation:** The LSTM-based temporal model successfully learned predictive patterns from the simulated 5G channel data, achieving a **validation accuracy of 55.1%**. This significantly outperforms random (3.4%) and majority-class (33%) baselines, establishing a strong proof-of-concept for the proactive, AI-driven approach.
*   **Energy Forecasting:** The neural network, utilizing a masked training strategy, achieved a **Mean Absolute Percentage Error (MAPE) of 5.74%** on the real-world dataset. This result is highly competitive with state-of-the-art models and confirms the high predictability of base station energy consumption.

## 🚀 A Researcher's Guide to Getting Started

If you are new to this project, here is a recommended workflow to get started:

1.  **Read the Thesis:** Begin by reading the main thesis draft located in `docs/Thesis_Draft.pdf`. This document provides the full context, literature review, methodology, and interpretation of the results.
2.  **Review the Documentation:** Explore the other files in the `docs/` folder, such as presentations and other content, for a high-level overview.
3.  **Set Up the Environments:** You will need two distinct environments: one for running the simulations (`ns-3`) and one for data analysis (`Python`). Follow the instructions in the sections below to set them both up.
4.  **Acquire/Generate the Datasets:**
    - Run the simulation to generate the link adaptation data (see instructions below).
    - Download the energy consumption dataset as described in the [`data/` folder section](#-data-folder).
5.  **Run the Analysis:** Explore the Jupyter notebooks in the `work/` folder to step through the data preprocessing, model training, and evaluation for each part of the thesis.

## 📂 Repository Structure

The repository is organized into three main folders: `docs`, `data`, and `work`.

### 📄 `docs/` Folder

This directory contains all supporting documentation.

- `Thesis_Draft.pdf`: The primary dissertation document.
- `presentations/`, `papers/`: Additional supporting materials.
- `graphs` - output graphs for reference

### 📊 `data/` Folder

This folder is the designated location for all datasets.

- `link_adaptation/`: This folder should contain the raw output logs from the 5G-LENA simulation.
- `energy/`: This folder is for the 5G energy consumption dataset.
- `effnet/`: Contains internal datasets from our industry partner, Effnet AB.

### 🔬 `work/` Folder

This is the main project folder containing all the executable code.

- `simulation_files/`: Contains the ns-3 C++ script (`opt-gsoc-nr-channel-models-error.cc`) and the Bash automation script (`run-multi-sim.sh`) required for data generation.
- `Link_Adaptation_Analysis.ipynb`: Jupyter Notebook for **Part I** of the thesis.
- `Energy_Forecasting_Analysis.ipynb`: Jupyter Notebook for **Part II** of the thesis.
- `requirements.txt`: A list of Python packages required for the notebooks.

## 🛠️ Environment Setup

### Part I-A: Simulation Environment (ns-3 & 5G-LENA)

To regenerate the dataset for the Predictive Link Adaptation part of the thesis, you must first set up the simulation environment. This project uses the **ns-3 Network Simulator** with the **5G-LENA** module.

The simulations were validated on **Ubuntu 20.04 LTS** with the **g++-11.4.0** toolchain.

**1. Install ns-3.44:**
This thesis requires `ns-3` version **3.44**. Follow the official `ns-3` installation guide for your platform. It is crucial to install all prerequisite libraries first.

- **Guide:** [ns-3 Installation Tutorial](https://www.nsnam.org/docs/tutorial/html/getting-started.html)

**2. Integrate 5G-LENA (NR-v4.0):**
This thesis uses `5G-LENA NR` version **4.0**. You need to download it and place it in the `contrib` directory of your `ns-3` installation.

- **Repository & Instructions:** [5G-LENA NR Module](https://github.com/5G-LENA/5G-LENA_fl-nr)
- Follow their instructions for downloading and building the module within your `ns-3` environment.

**3. Run the Automated Simulation Script:**
The dataset was generated by running the main simulation script multiple times with different random seeds. A bash script is provided to automate this entire process.

a. **Copy Simulation Files:** After building `ns-3` with `5G-LENA`, copy the necessary files from the `work/simulation_files/` directory of this repository to your `ns-3` root folder: - Copy `opt-gsoc-nr-channel-models-error.cc` into the `ns-3/scratch/` directory. - Copy `run-multi-sim.sh` into the main `ns-3/` root directory.

b. **Make the Script Executable:** Open a terminal in your `ns-3` root directory and run:
`bash
      chmod +x run-multi-sim.sh
      `

c. **Execute the Script:** Run the script from the `ns-3` root directory:
`bash
      ./run-multi-sim.sh
      `
This script will automatically run the simulation multiple times, create an output directory named `sim_results/`, and organize all generated log files into subfolders (e.g., `seed100_run1/`, `seed100_run2/`, etc.).

d. **Move Output for Analysis:** Once the script finishes, move the entire `sim_results/` directory into the `data/link_adaptation/` folder of this repository. The Jupyter notebook is configured to read the data from this location.

### Part I-B & II: Python Data Analysis Environment

1.  **Create a virtual environment (recommended):**
    ```bash
    python -m venv venv
    source venv/bin/activate  # On Windows, use `venv\Scripts\activate`
    ```
2.  **Install the dependencies:**
    ```bash
    pip install -r work/requirements.txt
    ```

## 📜 How to Cite

If you build upon this work, please cite the thesis:

```bibtex
@mastersthesis{Narendran2025,
  author = {Alka Valiparambil Narendran},
  title  = {AI for Link Adaptation and Energy Prediction in Realistic 5G Networks},
  school = {Leeds Beckett University},
  year   = {2025},
  month  = {August}
}
```
