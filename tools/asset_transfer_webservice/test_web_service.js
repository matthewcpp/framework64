const { program } = require("commander");

const fs = require("fs");

program
    .name("Asset Transfer Service Test Script")
    .version("1.0.0")
    .description("Sends asset paths to the transfer service");

program
    .command("mesh")
    .argument("<path>", "Path of Asset to send to server.")
    .argument("[uri]", "Uri of server to communicate with.", "http://127.0.0.1:62187")
    .action(transferStaticMesh);


async function transferStaticMesh(assetPath, serviceUri) {
    if (!fs.existsSync(assetPath)) {
        throw new Error(`Path does not exist: ${assetPath}`)
    }

    console.log(`Transfer static mesh: ${assetPath} --> ${serviceUri}`);

    const payload = {
        type: "mesh",
        src: assetPath
    };

    const response = await fetch(serviceUri, {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify(payload)
    });

    console.log(response.status);
}

program.parse();