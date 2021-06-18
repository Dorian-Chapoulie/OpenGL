const fs = require('fs');
const { read } = require('fs/promises');
const readline = require('readline');

const FILE = 'map_1.mtl';
const NEW_FILE = 'map.mtl';

let fileContent = "";

const reader = readline.createInterface({
    input: fs.createReadStream(FILE),
    output: undefined,
    console: false,
});

let lastMTL;

reader.on('line', function(line) {
  console.log(line)
    if (line.startsWith('newmtl')) {
      lastMTL = line.slice('newmtl'.length + 1);
      fileContent += line + '\n';
    } else if (line.startsWith('map_Kd')) {
      const file = lastMTL.replace('/', '_');
      fileContent += 'map_kd textures/' + file + '.png\n';
    } else if (line.startsWith('map_Ks')) {
      const file = lastMTL.replace('/', '_');
      fileContent += 'map_ks textures/' + file + '.png\n';
    } else {
      fileContent += line + '\n';
    }
});

reader.on('close', () => {
  console.log("closed", fileContent)
  fs.writeFileSync(NEW_FILE, fileContent);
});