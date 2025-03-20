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

When you get to the make step, make sure to run it with multiple cores using:

```
make -j$(nproc)
```

The `nproc` command automatically gets your system core count.

If you are using the 5.15 kernel, I highly recommend using the provided `.config` file from this directory as it already has all the required modifications needed to perform a successful kernel install. You can copy it over during the step that mentions kernel configurations. 

If installing the kernel without this `.config` (perhaps on a differnt system), make sure to disable system trusted keys:

Find `CONFIG_SYSTEM_TRUSTED_KEYS="debian/canonical-certs.pem"` and change it to:

```
CONFIG_SYSTEM_TRUSTED_KEYS=""
```

Also, when compiling the kernel, you might run into a `Failed to load BTF from vmlinux` error. Some troubleshooting tips for this would be:

- Check `.config` to see if `CONFIG_DEBUG_INFO_BTF` is enabled. If it is, make sure to disable it by setting it to `=n` or running:

```
scripts/config --disable CONFIG_DEBUG_INFO_BTF
```

Then run:

```
make oldconfig
```

And try rebuilding. 

> **_NOTE:_** The provided `.config` already has this disabled, so if you end up running into this error, I recommend replacing the `.config` with the one provided in this repo.

If only performing minor changes, I recommend not performing `make clean` every time you start a new make, as the compiling for the drivers can take a significant amount of time.

---

## Final Steps for Kernel Setup

Once that is finished, you can follow the rest of the steps mentioned on Mark’s page site. Additionally, make sure to run:

```
sudo make headers_install INSTALL_HDR_PATH=usr/include
```

This will set up the appropriate headers. I would run this after your make of the kernel completes. Once you finish all the steps and complete the reboot, you should have the ExtMem custom kernel all set up!

---

## Building ExtMem

Once you have the kernel installed, you will need to set up the `libsyscall_intercept` library. To do this:

1. Clone the repo:

```
git clone https://github.com/pmem/syscall_intercept
```

2. Install `cmake`:

```
sudo apt install cmake
```
* Note: There might be other libraries you need to install, the build process will error and describe these dependencies, simply sudo apt install them then remake. 

3. Follow the instructions on their repo to build the library.

With that complete, you should be able to build ExtMem. You can follow their build commands below:

```
make libextmem-default.so  # make default policy
make all                   # all policies
```

---

## Running ExtMem

With ExtMem now built, you can follow the steps listed on their GitHub for running programs with the ExtMem features.

### Some notes on running programs:
For Cloudlab, the typical `SWAPDIR` is `/dev/sda99`, but you can use:

```
swapon --show
```

To see stats on your current partition or:

```
lsblk
```

To get a list of all partitions.

I would recommend looking at their pagerank policy code as that is where they have implemented their custom memory policy. This is a good refrence whem creating your own custom policy. The file for this is found at https://github.com/SepehrDV2/ExtMem/tree/master/src/policies

You can find a test script that runs programs with ExtMem on CloudLab in the `ExtMem_Tests` directory (`test_script_ExtMem.sh`).

---

## Congratulations!

ExtMem should be fully set up and running on Cloudlab now. Sample test programs are provided in `ExtMem_Tests` for you to try out to get a feel for operating with ExtMem.

