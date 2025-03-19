# Guide to Setting Up ExtMem on Cloudlab

## Resources
- [Cloudlab Getting Started Guide](https://docs.cloudlab.us/getting-started.html)
- [Kernel Build on Cloudlab](https://pages.cs.wisc.edu/~markm/kernel-build-cloudlab.html)

## Setting Up Cloudlab

Begin by setting up your Cloudlab account. You can follow the guide linked in the resources section, which walks you through creating an account on Cloudlab. This guide also contains valuable information about Cloudlab's features, which will be helpful later on.

Once you have completed creating the group project and are ready to set up your first experiment, you will first encounter the **Select Profile** page. It is recommended to use the default **'small-lan'** profile. A custom profile can be set up later if you plan to run frequent experiments.

### Experiment Setup
1. **Select Parameters:**
   - Choose the number of nodes you will be using.
   - Select an OS image. For ExtMem, Ubuntu 22.04 is recommended, as it natively runs the 5.15 Linux kernel (the version ExtMem uses). Any OS running kernel version 5.15 or higher should work.
   - Ensure the physical node type is an x86 multicore machine.
   - Other settings can be left as default.
   - Set the start time and duration for the experiment, or leave the start time blank to create it immediately.

### Accessing the Experiment
Once the experiment is ready, you can SSH into it using the provided SSH command in the **List View**.

### Setting Up ExtMem
1. Clone and set up the ExtMem repository. You can follow the **Getting Started** section on the ExtMem GitHub page for this part.
2. When updating submodules, use the following command to display progress (as it can take some time):
   ```sh
   git submodule update --init --progress
   ```
3. Run the first two `apt install` commands to install the necessary libraries.

### Custom Kernel Setup
From this point, it is recommended to follow the custom kernel setup guide provided [here](https://pages.cs.wisc.edu/~markm/kernel-build-cloudlab.html) (linked in the resources section). This guide provides a cleaner setup of custom kernels specifically tailored for Cloudlab, and has been more reliable in practice.

---

By following these steps, you should have ExtMem set up and running on Cloudlab successfully!


