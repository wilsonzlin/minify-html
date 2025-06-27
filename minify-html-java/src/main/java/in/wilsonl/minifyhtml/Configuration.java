package in.wilsonl.minifyhtml;

import lombok.AccessLevel;
import lombok.AllArgsConstructor;
import lombok.Builder;

/**
 * Class representing minification configuration.
 * Use the {@link Builder} to create an instance of this class.
 */
@Builder(
        setterPrefix = "set",
        builderClassName = "Builder"
)
@AllArgsConstructor(access = AccessLevel.PRIVATE)
public class Configuration {
    public final boolean allowNoncompliantUnquotedAttributeValues;
    public final boolean allowOptimalEntities;
    public final boolean allowRemovingSpacesBetweenAttributes;
    public final boolean keepClosingTags;
    public final boolean keepComments;
    public final boolean keepHtmlAndHeadOpeningTags;
    public final boolean keepInputTypeTextAttr;
    public final boolean keepSsiComments;
    public final boolean minifyCss;
    public final boolean minifyDoctype;
    public final boolean minifyJs;
    public final boolean preserveBraceTemplateSyntax;
    public final boolean preserveChevronPercentTemplateSyntax;
    public final boolean removeBangs;
    public final boolean removeProcessingInstructions;
}
