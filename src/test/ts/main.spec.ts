import chai, {expect} from "chai";
import "mocha";
import {hyperbuild, HyperbuildError} from "../../main/ts/main";
import chaiAsPromised = require("chai-as-promised");

chai.use(chaiAsPromised);

describe("hyperbuild", () => {
  it("should trim whitespace", () => {
    let res = hyperbuild({
      code: "<h1>    a    </h1>"
    });
    return expect(res).to.eventually.deep.equal({
      code: "<h1>a</h1>",
    });
  });

  it("should destroy whole whitespace", () => {
    let res = hyperbuild({
      code: `<div>
        <section></section>
      
        <section>
          <h1>  Helloo  </h1>
        </section>
      </div>`,
    });
    return expect(res).to.eventually.deep.equal({
      code: "<div><section></section><section><h1>Helloo</h1></section></div>",
    });
  });

  it("should throw an error on malformed entities", () => {
    let res1 = hyperbuild({
      code: `<div>&x10FFF;</div>`,
    });
    let res2 = hyperbuild({
      code: `<div>Johnson & Johnson</div>`,
    });
    let res3 = hyperbuild({
      code: `<div>&10FFFF;</div>`,
    });
    let res4 = hyperbuild({
      code: `<div>&mdash</div>`,
    });

    [res1, res2, res3, res4].forEach(p => {
      expect(p).to.eventually.be.rejected
        .and.be.an.instanceOf(HyperbuildError)
        .and.have.property("code", 65);
    });
  });

  it("should decode valid entities", () => {
    let res1 = hyperbuild({
      code: `<div>&#x10FFF;</div>`,
    });
    expect(res1).to.eventually.deep.equal({
      code: `<div>\u{10FFF}</div>`,
    });

    let res2 = hyperbuild({
      code: `<div>&apos;</div>`,
    });
    expect(res2).to.eventually.deep.equal({
      code: `<div>'</div>`,
    });
  });
});
