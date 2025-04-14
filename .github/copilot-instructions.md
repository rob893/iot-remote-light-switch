Remote Light Switcher

An application that allows me to turn off and on a light switch in my house using an IOT control board with a motor attached to it which pings a server to get the state of the light switch. I can change the state via a web application.

- @azure Rule - Use Azure Best Practices: When generating code for Azure, running terminal commands for Azure, or performing operations related to Azure, invoke your `azure_development-get_best_practices` tool if available.

Technologies used...

- The IOT device uses arduino IDE with a Node MCU ESP-12E development board and a SG90 micro servo motor attached to it.
- The backend server is written in .NET 8 and deployed to azure app service
- The web app is a React SPA using vite, typescript, and tailwind css and will be deployed to Github Pages

The project is broken down into these folders:

./CI - This is where CI files like bicep goes
./LightSwitchApi - this is where the server code goes
./LightSwitcherEmbedded - this is where it IoT device code goes
./light-switch-ui - This is where the react SPA code goes
