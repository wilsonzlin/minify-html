import {expect} from "chai";
import "mocha";

const {load: loadHyperbuild, hyperbuild} = require("../../hyperbuild.em.js");

before(() => {
  return loadHyperbuild();
});

describe("hyperbuild", () => {
  it("should trim whitespace", () => {
    expect(hyperbuild({
      inputCode: `<h1>    a    </h1>`,
    })).to.equal("<h1>a</h1>");
  });

  it("should destroy whole whitespace", () => {
    expect(hyperbuild({
      inputCode: `<div>
          <section></section>

          <section>
            <h1>  Helloo  </h1>
          </section>
        </div>`,
    })).to.equal("<div><section></section><section><h1>Helloo</h1></section></div>");
  });

  it("should destroy whole whitespace at root", () => {
    expect(hyperbuild({
      inputCode: `
        <div>
            <section></section>
        </div>

        <div>
        </div>
      `,
    })).to.equal("<div><section></section></div><div></div>");
  });

  it("should throw an error on malformed entities", () => {
    [
      `<div>&x10FFF;</div>`,
      `<div>Johnson & Johnson</div>`,
      `<div>&10FFFF;</div>`,
      `<div>&mdash</div>`,
    ].map(p => {
      let err = null;
      try {
        hyperbuild({inputCode: p});
      } catch (e) {
        err = e;
      }

      expect(err)
        .and.have.property("code", 65);
    });
  });
});

it("should decode valid entities", () => {
  expect(hyperbuild({
    inputCode: `<div>&#x10FFFF;</div>`,
  })).to.equal(`<div>\u{10FFFF}</div>`);

  expect(hyperbuild({
    inputCode: `<div>&apos;</div>`,
  })).to.equal(`<div>'</div>`);
});
