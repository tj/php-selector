<?php

function select_elements($selector, $html) {
  $dom = new DOMDocument();
  $dom->loadHTML($html);
  $xpath = new DOMXpath($dom);
  return elements_to_array($xpath->evaluate(selector_to_xpath($selector)));
}

function elements_to_array($elements) {
  $array = array();
  for ($i = 0, $length = $elements->length; $i < $length; ++$i)
    if ($elements->item($i)->nodeType == XML_ELEMENT_NODE)
      array_push($array, element_to_array($elements->item($i)));
  return $array;
}

function element_to_array($element) {
  $array = array(
    'name' => $element->nodeName,
    'attributes' => array(),
    'text' => $element->textContent,
    'children' => elements_to_array($element->childNodes)
    );
  foreach((array) $element->attributes as $key => $attr)
    $array['attributes'][$key] = $attr->value;
  return $array;
}

function selector_to_xpath($selector) {
  $selector = 'descendant-or-self::' . $selector;
  // :button, :submit, etc
  $selector = preg_replace('/:(button|submit|file|checkbox|radio|image|reset|text|password)/', 'input[@type="\1"]', $selector);
  // [id]
  $selector = preg_replace('/\[(\w+)\]/', '*[@\1]', $selector);
  // foo[id=foo]
  $selector = preg_replace('/\[(\w+)=[\'"]?(.*?)[\'"]?\]/', '[@\1="\2"]', $selector);
  // [id=foo]
  $selector = str_replace(':[', ':*[', $selector);
  // div#foo
  $selector = preg_replace('/([\w\-]+)\#([\w\-]+)/', '\1[@id="\2"]', $selector);
  // #foo
  $selector = preg_replace('/\#([\w\-]+)/', '*[@id="\1"]', $selector);
  // div.foo
  $selector = preg_replace('/([\w\-]+)\.([\w\-]+)/', '\1[contains(@class,"\2")]', $selector);
  // .foo
  $selector = preg_replace('/\.([\w\-]+)/', '*[contains(@class,"\1")]', $selector);
  // div:first-child
  $selector = preg_replace('/([\w\-]+):first-child/', '*/\1[position()=1]', $selector);
  // div:last-child
  $selector = preg_replace('/([\w\-]+):last-child/', '*/\1[position()=last()]', $selector);
  // :first-child
  $selector = str_replace(':first-child', '*/*[position()=1]', $selector);
  // :last-child
  $selector = str_replace(':last-child', '*/*[position()=last()]', $selector);
  // div:nth-child
  $selector = preg_replace('/([\w\-]+):nth-child\((\d+)\)/', '*/\1[position()=\2]', $selector);
  // :nth-child
  $selector = preg_replace('/:nth-child\((\d+)\)/', '*/*[position()=\1]', $selector);
  // :contains(Foo)
  $selector = preg_replace('/([\w\-]+):contains\((.*?)\)/', '\1[contains(string(.),"\2")]', $selector);
  // >
  $selector = preg_replace('/\s*>\s*/', '/', $selector);
  // ~
  $selector = preg_replace('/\s*~\s*/', '/following-sibling::', $selector);
  // + 
  $selector = preg_replace('/\s*\+\s*([\w\-]+)/', '/following-sibling::\1[position()=1]', $selector);
  // ' '
  $selector = preg_replace('/\s+/', '/descendant::', $selector);
  $selector = str_replace(']*', ']', $selector);
  $selector = str_replace(']/*', ']', $selector);
  return $selector;
}