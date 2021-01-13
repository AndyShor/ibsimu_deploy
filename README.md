# IBSIMU deploy pack for AWS installation with CloudFormation
This is set of tools to create infrastructure on AWS, deploy IBSIMU and configure remote GUI use.

![example optics](/images/ibsimu-stack_small.png)

Use as following.
Use ibsimu_stack.json to create stack with CloudFormation. This version uses hardcoded AMI for Ubuntu 20.04 in eu-central-1 AZ.
Replace AMI as well as auth key. Presently seciurity group allows inbound traffic on ports 22 and 5901 from any IP. 
Feel free to be more restrictive. Bootstrapping will take several minutes even after you will be able to ssh to the instance.
Using SCP transfer `install_ibsimu.sh`, `vnc_settings.sh`, and `vnc_setup.sh` to `/home/ubuntu`.
Wait for bootstrapping to complete in the background under root (watch for 'bootstraping complete' in `/diagnostics.log`).
Change permissions of `install_ibsimu.sh` and `vnc_setup.sh` with `chmod +x`.
Run `./install_ibsimu`, it will take several minutes with compilations and tests.
Run `./vnc_setup`, to install and configure vnc server.
After installation you will be able to connect to your desktop using port 5901 using for exanple tightvnc server on Windows.
SCP simple test case such as einzel lens, make sure that Makefile in the case refers to ibsimu1.0.6dev version.
Make and run the case.


A more in-depth explanation of what is going on during bootstraping (and why) is given in an upcoming blog post.
