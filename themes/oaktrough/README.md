# OAKTROUGH

OAKTROUGH is a Hugo theme based on semantic HTML elements and simple, ultra-responsive
CSS.

## Title Override

Want your page title to be one thing but the page itself to show something else, like a
name for a resumé? Use `display_title` in the front matter. The `title` will
still show up in the title bar and as the `title` attribute for all metadata.

Need to hide post metadata? Try `hide_post_meta: true`.

## Skip to Main Content

Every page has a "skip to main content" as its first link, for better screenreader and
keyboard support.

## Navigation

All navigation is placed at the top of the page beside the title.

## Post Metadata
Every post contains a few `<meta>` fields, which correspond to either site config or
post frontmatter variables. These are Dublin Core enabled where possible, meaning that
posts on this theme can easily be ingested into e.g. Zotero.

- `Title` is the title of the post.
- `Creator` is from `author` in the frontmatter or `params.Author` in `config.toml` otherwise.
- `Date` is the Hugo-recorded date of the post.
- `Language` is from Hugo's generating locale.
- `Description` is from `Description` in the frontmatter, or `Summary` if that's not present, or `MetaSiteDescription` in `config.toml` as a fallback.
- `Identifier` is the permalink URL.

Non-Dublin Core attributes are:

- `viewport` for rendering properly, this isn't customizable
- `keywords` is from the `tags` taxonomy in the frontmatter
- `google-site-verification` is from `params.googlesiteverification` in `config.toml` in case you need that.
- `nolist` turns off listing sub-pages on specific index pages.

## Figures

To insert a figure with caption, you can use the `{{< figure >}}` shortcode. Simply
place your Markdown with image and caption inside it. To make it float left or right,
use `{{< figure "left" >}}` or `{{< figure "right" >}}`. For instance:

```markdown
{{< figure "right" >}}                                                                                                                                                                    
    ![The drive cage of my Thelio with one drive out of its slot.](/images/thelio/drive_cage.jpg)
    Drives are inserted on rails using vibration-isolating gromits. If only it were this easy 
    to get RAID working!                                                                      
{{< /figure >}}   
```

