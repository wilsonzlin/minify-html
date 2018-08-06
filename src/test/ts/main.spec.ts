import chai, { expect } from "chai";
import "mocha";
import { hyperbuild, HyperbuildError } from "../../main/ts/main";
import chaiAsPromised = require("chai-as-promised");

chai.use(chaiAsPromised);

const DEFAULT_RESULT = {
  messages: {
    debug: [],
    info: [],
    unknown: [],
  },
  warnings: [],
};

describe("hyperbuild", () => {
  it("should trim whitespace", () => {
    return expect(hyperbuild({
      code: `<h1>    a    </h1>`,
    })).to.eventually.deep.equal({
      ...DEFAULT_RESULT,
      code: "<h1>a</h1>",
    });
  });

  it("should destroy whole whitespace", () => {
    return expect(hyperbuild({
      code: `<div>
        <section></section>

        <section>
          <h1>  Helloo  </h1>
        </section>
      </div>`,
    })).to.eventually.deep.equal({
      ...DEFAULT_RESULT,
      code: "<div><section></section><section><h1>Helloo</h1></section></div>",
    });
  });

  it("should throw an error on malformed entities", () => {
    return Promise.all([
      hyperbuild({
        code: `<div>&x10FFF;</div>`,
      }),
      hyperbuild({
        code: `<div>Johnson & Johnson</div>`,
      }),
      hyperbuild({
        code: `<div>&10FFFF;</div>`,
      }),
      hyperbuild({
        code: `<div>&mdash</div>`,
      }),
    ].map(p => {
      return expect(p).to.eventually.be.rejected
        .and.be.an.instanceOf(HyperbuildError)
        .and.have.property("code", 65);
    }));
  });

  it("should decode valid entities", () => {
    return Promise.all([
      expect(hyperbuild({
        code: `<div>&#x10FFF;</div>`,
      })).to.eventually.deep.equal({
        ...DEFAULT_RESULT,
        code: `<div>\u{10FFF}</div>`,
      }),

      expect(hyperbuild({
        code: `<div>&apos;</div>`,
      })).to.eventually.deep.equal({
        ...DEFAULT_RESULT,
        code: `<div>'</div>`,
      }),
    ]);
  });
});
