(function() {
  "use strict";

  let lines = [];
  document.querySelectorAll(".named").forEach(t => {
    let hex = t.nextElementSibling.textContent.slice(3, -1);
    t.textContent.trim().split(/\s+/).map(raw => {
      let r = raw.slice(1, -1);
      lines.push([r, Number.parseInt(hex, 16)]);
    });
  });

  console.log(lines.map(l => {
    return `nh_map_str_int32_set(hbr_entityrefs_map, "${l[0]}", 0x${l[1].toString(16)});`
  }).sort().join("\n"));
})();
