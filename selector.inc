<?php

// --- Selector.inc - (c) Copyright TJ Holowaychuk <tj@vision-media.ca> MIT Licensed

define('SELECTOR_VERSION', '1.1.3');

/**
 * SelectorDOM.
 *
 * Persitant object for selecting elements.
 *
 *   $dom = new SelectorDOM($html);
 *   $links = $dom->select('a');
 *   $list_links = $dom->select('ul li a');
 *
 */

class SelectorDOM {
  public function SelectorDOM($html) {
    $this->html = $html;
    $this->dom = new DOMDocument();
    @$this->dom->loadHTML($html);
    $this->xpath = new DOMXpath($this->dom);
  }
  
  public function select($selector, $as_array = true) {
    $elements = $this->xpath->evaluate(selector_to_xpath($selector));
    return $as_array ? elements_to_array($elements) : $elements;
  }
}

/**
 * Select elements from $html using the css $selector.
 * When $as_array is true elements and their children will
 * be converted to array's containing the following keys (defaults to true):
 *
 *  - name : element name
 *  - text : element text
 *  - children : array of children elements
 *  - attributes : attributes array
 *
 * Otherwise regular DOMElement's will be returned.
 */

function select_elements($selector, $html, $as_array = true) {
  $dom = new SelectorDOM($html);
  return $dom->select($selector, $as_array);
}

/**
 * Convert $elements to an array.
 */

function elements_to_array($elements) {
  $array = array();
  for ($i = 0, $length = $elements->length; $i < $length; ++$i)
    if ($elements->item($i)->nodeType == XML_ELEMENT_NODE)
      array_push($array, element_to_array($elements->item($i)));
  return $array;
}

/**
 * Convert $element to an array.
 */

function element_to_array($element) {
  $array = array(
    'name' => $element->nodeName,
    'attributes' => array(),
    'text' => $element->textContent,
    'children' =>elements_to_array($element->childNodes)
    );
  if ($element->attributes->length)
    foreach($element->attributes as $key => $attr)
      $array['attributes'][$key] = $attr->value;
  return $array;
}

/**
 * Convert $selector into an XPath string.
 */

function selector_to_xpath($selector) {
  $selector = 'descendant-or-self::' . $selector;
  // ,
  $selector = preg_replace('/\s*,\s*/', '|descendant-or-self::', $selector);
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
