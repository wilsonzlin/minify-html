package in.wilsonl.minifyhtml;

import lombok.AccessLevel;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Value;

/**
 * Class representing minification configuration.
 * Use the {@link Builder} to create an instance of this class.
 */
@AllArgsConstructor(access = AccessLevel.PRIVATE)
@Builder(
    setterPrefix = "set",
    builderClassName = "Builder"
)
@Value
public class Configuration {
    /* BEGIN FIELD NAMES */
    boolean allowNoncompliantUnquotedAttributeValues;
    boolean allowOptimalEntities;
    boolean allowRemovingSpacesBetweenAttributes;
    boolean keepClosingTags;
    boolean keepComments;
    boolean keepHtmlAndHeadOpeningTags;
    boolean keepInputTypeTextAttr;
    boolean keepSsiComments;
    boolean minifyCss;
    boolean minifyDoctype;
    boolean minifyJs;
    boolean preserveBraceTemplateSyntax;
    boolean preserveChevronPercentTemplateSyntax;
    boolean removeBangs;
    boolean removeProcessingInstructions;
    /* END FIELD NAMES */
}
