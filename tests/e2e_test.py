import subprocess

# --- Configuration ---
LCSF_GEN_BIN_PATH = '../LCSF_Generator/build/bin/lcsf_generator_cli'
TEST_PROT_DESC_PATH = '../LCSF_Generator/example/Test.json'
LCSF_GEN_A_INPUT_PATH = './src/protocols_a/Test_Main_a.c'
LCSF_GEN_B_INPUT_PATH = './src/protocols_b/Test_Main_b.c'

# --- 1. Run the Generator ---
cmd_gen = f'{LCSF_GEN_BIN_PATH} -l {TEST_PROT_DESC_PATH} -a {LCSF_GEN_A_INPUT_PATH} -b {LCSF_GEN_B_INPUT_PATH}'

print(f"Executing: {cmd_gen}")
subprocess.run(cmd_gen, shell=True, check=True)

# --- 2. Build and Run Tests ---
cmd_build_test = 'cmake --build build/ --target run_tests'

print(f"Executing: {cmd_build_test}")
subprocess.run(cmd_build_test, shell=True, check=True)
